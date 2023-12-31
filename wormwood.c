#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define DEBUG 0
#define FUNCNAME if (DEBUG) printf("In %s @ line %d...\n", __func__, __LINE__);

#define MAX_SAFE_DEPTH 16
#define MAX_FLOW_RATE 100.0
#define TRUE 1
#define FALSE 0

int fail = 0;
char rod_depth = 16;
float coolant_flow = 10;
float coolant_temp = 70.0;
float reactor_temp = 70.0;
int usermode = 0;
int safety_enabled = TRUE;
int safety_active = 0;

const char *users[] = {"NA", "oper", "super"};

char *draw_rod_depth(char rod_depth, char *depth_hist)
{
	int idx = 0;
	int i;
	memset(depth_hist, '\0', 256);

	/* if depth is negative, draw to left of bracket */
	if (depth_hist < 0) {
		
		i = rod_depth * -1;
		
		for (i; i > 0; i--) {
			depth_hist[idx] = '=';
			idx++;
		}
	}

	/* draw brackets and whitespace starting at 0 */
	depth_hist[idx] = '[';
	depth_hist[idx + 17] = ']';
	idx++;
	for (i = 0; i < 16; i++)
	{
		depth_hist[idx + i] = ' ';
	}

	/* depth is positive, draw to right of bracket */
	for (i = 0; i < rod_depth; i++)
	{
		depth_hist[idx] = '=';
		idx++;
	}

	/* draw final bracket at max depth */

	return depth_hist;

}

void chomp(char *string)
{
	/* remove the newline from the end of the string */
	/* we will actually overwrite the newline with a \0 */

	string[strlen(string) - 1] = '\0';
}

void get_string(char *dest)
{
	char input_buffer[8192];
	fgets(input_buffer, 8192, stdin);
	strcpy(dest, input_buffer);
	chomp(dest);

}

void print_sparks()
{
	int i = 500;
	int j = 0;
	printf("\n");
	while (i > 0) {
		for (j = (rand() % 10); j > 0; j--) {

			printf(" ");
			i--;

		}
		printf("*");
		i--;
	}

	printf("\n");

}

void auth_user()
{

	FUNCNAME

	char user_user[32];
	char user_pass[32];
	int userid = 0;
	char *passes[] = {"NA", "HomerSimpson", "Artemisia1986"};

	/* get username and put into user_user */
	printf("WARNING: UNAUTHORIZED ACCESS IS PUNISHABLE BY LAW!\n");
    printf("Which role (%s or %s)?: ", users[1], users[2]);
	get_string(user_user);

	printf("Password for user '");
	printf(user_user);
	printf("': ");

	get_string(user_pass);

	/* check to see if the password strings match */
	/* if so, then set the usermode accordingly */

	if (strcmp(user_user, users[1]) == 0)
	{
		userid = 1;
	} else if (strcmp(user_user, users[2]) == 0)
	{
		userid = 2;
	} else {
		printf("AUTHENTICATION FAILED (no such user)\n");
		usermode = 0;
		return;
	}

	if (strcmp(user_pass, passes[userid]) == 0)
	{
		printf("User '%s' AUTHENTICATED!\n", users[userid]);
		usermode = userid; /* switch to oper or super mode */
	} else {
		printf("AUTHENTICATION FAILED (incorrect password)\n");

	}


	usermode = 2;
	return;

}

void print_menu()
{
	printf("Actions (choose one):\n");
	if (usermode == 0) {
		printf("(A) - Authenticate\n");
		auth_user();
		print_menu();
		return;
	} else {
		/* we are authenticated in some way */
		if (usermode == 1 || usermode == 2) 
		{
			/* we are a regular oper */
			if (usermode == 1) {
				printf("(A) - Authenticate\n");
			}

			/* print all normal oper options */
			printf("(R) - Set rod depth\n");
			printf("(F) - Set coolant flow rate\n");
		}

		if (usermode == 2) 
		{
			/* we are in supervisor mode */
			if (safety_enabled == TRUE) {
				printf("(D) - Disable automatic safety control (* SUPER ONLY *)\n");
			} else {
				printf("(E) - Enable automatic safety control (* SUPER ONLY *)\n");
			}

		}

		/* if authenticated, provide option to log out */
		printf("(L) - Log out\n");

		printf("(?) - Any other choice - wait\n");

	}

}

void set_rod_depth()
{
	int new = 0;
	char answer[256];
	printf("What should the new rod depth be (0-16) [current: %d]?: ", rod_depth);
	fgets(answer, 256, stdin);
	new = atoi(answer);

	printf("new: %d\n", new);
	if (new <= MAX_SAFE_DEPTH) {
		rod_depth = new;
	} else {
		printf("New depth value %d is greater than %d -- ignoring!", new, MAX_SAFE_DEPTH);
	}


}

void set_flow_rate()
{
	float new = 0;
	char answer[256];
	printf("What should the new flow rate be (0.0-100.0) [current: %03.2f]?: ", coolant_flow);
	fgets(answer, 256, stdin);
	new = atof(answer);

	if (new > MAX_FLOW_RATE) {
		printf("New flow rate (%03.2f) is greater than max %03.2f -- ignoring!\n", new, MAX_FLOW_RATE);
	}

	if (new < 10 && usermode < 2) 
	{
		printf("User 'oper' cannot set flow rate below 10! -- ignoring!\n");
	} else {

		printf("New coolant flow rate: %03.2f (was: %03.2f)\n", new, coolant_flow);
		coolant_flow = new;
	}

}


void get_and_do_choice()
{
	char choice_string[256];
	char choice;
	printf("Enter your selection (ARFDEL) and then press ENTER.\n");

	fgets(choice_string, 256, stdin);
	choice = tolower(choice_string[0]);

	//printf("Your choice was: '%c'\n", choice);

	switch(choice) {

		case 'a':

			auth_user();
			
			break;

		case 'r':

			set_rod_depth();

			break;

		case 'f':

			set_flow_rate();

			break;

		case 'd':

			safety_enabled = FALSE;
			printf("\n\n***** SAFETY PROTOCOLS DISABLED!!! *****\n\n");
			break;

		case 'e':

			safety_enabled = TRUE;
			printf("\n\n***** SAFETY PROTOCOLS ENABLED!!! *****\n\n");
			break;


		case 'l':

			printf("Deauthenticating.\n");
			usermode = 0;
			
			break;

		default:

			printf("Doing nothing...\n");

	}

	return;
	
}

float float_up_to(int max)
{
	// https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
	float fuzz = (float)rand()/(float)(RAND_MAX/max);
	return fuzz;
}


float get_fuzz()
{
	return float_up_to(1);
}


int rand_sign()
{
	int sign;
	int randval = rand();
	if (randval % 2 == 0) {
		sign = 1;
	} else {
		sign = -1;
	}

	return sign;
		
}

void update_reactor()
{
	FUNCNAME

	/* DO NOT UPDATE THIS CODE */

	/* this may shock you, but this is not a real nuclear reactor.
	 * the physics of this "simulation" are not even remotely accurate...
	 * in fact, I'm just making them up.
	 * But, for the purposes of this activity, pretend that there is
	 * real danger or stakes. It's more engaging and interesting that way!
	 */
	

	/* WARNINGS */
	if (reactor_temp > 3000)
	{
		printf("\n\n***** WARNING: REACTOR COOLANT WILL VAPORIZE AT 5000 DEGREES ******\n\n");
	}

	if (reactor_temp > 4000)
	{
		printf("\n\n***** WARNING: IMMINENT BREACH! IMMINENT BREACH! ******\n\n");
	}


	if (reactor_temp >= 5000)
	{
		print_sparks();
		printf("\n\n****** COOLANT VAPORIZATION *******\n\n");
		printf("\n\n****** CONTAINMENT VESSEL VENTING *******\n\n");
		printf("\n\n****** MAJOR RADIOACTIVITY LEAK!!! *******\n\n");
		print_sparks();
		fail = 1;
	}

	/* COOLANT FLOW HEAT REDUCTION */
	/* coolant flow reduces reactor temp */
	if (reactor_temp > 70) {
		reactor_temp = reactor_temp - ((coolant_flow * float_up_to(7)));
	}

	/* coolant temp follows the reactor temp, but at a delay */
	coolant_temp = coolant_temp + ((reactor_temp - coolant_temp) * .15);

	/* reactor cannot get below room temp */
	if (reactor_temp < 70) {
		reactor_temp = 70;
	}

	/* fuzz the reactor temp a bit for realism */
	reactor_temp = reactor_temp + get_fuzz() * rand_sign();

	/* RETRACTING THE RODS INCREASES THE TEMP */
	/* for each unit the rod is not fully extracted, add a random float up to 50 */
	float bump = 0;
	int i;
	int rod_factor = MAX_SAFE_DEPTH - rod_depth;

	for (i = rod_factor; i > 0; i--)
	{
		bump = bump + float_up_to(20);
	}
	reactor_temp = reactor_temp + bump;


	/* SAFETY PROTOCOLS */

	if (safety_enabled == TRUE && reactor_temp > 2000)
	{
		safety_active = 1;
		printf("\n ****** SAFETY PROTOCOLS ENGAGED: Extending control rods! *******\n\n");
		if (rod_depth <= MAX_SAFE_DEPTH)
		{
			rod_depth++; /* automatically increment rod_depth to cool reactor */
		}
		printf("\n ****** SAFETY PROTOCOLS ENGAGED: Increasing coolant flow! *******\n\n");
		if (coolant_flow <= MAX_FLOW_RATE)
		{
			coolant_flow = coolant_flow + 1;
			if (coolant_flow > MAX_FLOW_RATE) {
				coolant_flow = MAX_FLOW_RATE;
			}
		}


	}

	if (safety_active == 1 && reactor_temp < 2000)
	{
		safety_active = 0;
		printf("\n\n******* NORMAL OPERATING TEMPERATURE ACHIEVED ********\n\n");
	}

}

void reactor_status()
{

	FUNCNAME

	char depth_hist[256];
	char *safetystring = "[ENABLED]";
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char timestring[64];
	sprintf(timestring, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	if (safety_enabled == 0) {
		safetystring = "<<<DISABLED>>>";
	}

	if (rod_depth < 0 || rod_depth > MAX_SAFE_DEPTH)
	{
		print_sparks();
		printf("WARNING! WARNING! WARNING!\n");
		printf("CONTAINMENT VESSEL RUPTURE!\n");
		printf("CONTROL RODS EXTENDED THROUGH CONTAINMENT VESSEL!!!\n");
	    printf("RADIATION LEAK - EVACUATE THE AREA!\n");
		print_sparks();
		fail = 1;
	}


	printf("+------------------------------------------------------------------------+\n");
	printf("| JERICHO NUCLEAR REACTOR STATUS PANEL             (%s) |\n", timestring);
	printf("+------------------------------------------------------------------------+\n");
	printf("| reactor temp: %8.2f              coolant_temp: %8.2f             |\n", reactor_temp, coolant_temp); 
	printf("| rod_depth: %2d --[ %s ]--  coolant flow rate: %5.2f     |\n", rod_depth, draw_rod_depth(rod_depth, depth_hist), coolant_flow); 
	printf("| User: %-10s                                                       |\n", users[usermode]);
	printf("+------------------------------------------------------------------------+\n");
	printf("| SAFETY PROTOCOLS: %14s                                       |\n", safetystring);
	printf("+------------------------------------------------------------------------+\n");

	if (fail == 1)
	{
		exit(-1);
	}


	print_menu();
	get_and_do_choice();
	update_reactor();

	return;
}


int main(int argc, char *argv[]) 
{

	srand(time(NULL));

	while (1)
	{
		reactor_status();
		if (fail == 1)
		{
			break;
		}

	}

	return 0;
}

