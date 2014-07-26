/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "addresses.h"
#include "game.h"
#include "date.h"
#include "rct2.h"
#include "string_ids.h"
#include "util.h"

// TODO Store real names in a data file.
#pragma region Real names

const char *real_names[] = {
	"Aaron",
	"Abdul",
	"Abraham",
	"Abu",
	"Adam",
	"Adrian",
	"Adriane",
	"Aileen",
	"Aisha",
	"Akiko",
	"Akira",
	"Al",
	"Ali",
	"Alan",
	"Alana",
	"Albert",
	"Alberta",
	"Alec",
	"Alesia",
	"Alex",
	"Alexa",
	"Alexander",
	"Alexandra",
	"Alexis",
	"Alf",
	"Alfonso",
	"Alfred",
	"Alice",
	"Alicia",
	"Alison",
	"Alistair",
	"Allan",
	"Allen",
	"Allison",
	"Allister",
	"Alma",
	"Alvin",
	"Alyson",
	"Amanda",
	"Amber",
	"Amilio",
	"Amos",
	"Amy",
	"Ana",
	"Anabel",
	"Anastasia",
	"Andie",
	"Andrea",
	"Andres",
	"Andrew",
	"Andy",
	"Angel",
	"Angela",
	"Angelica",
	"Angie",
	"Angus",
	"Anika",
	"Ann",
	"Anna",
	"Anne",
	"Annette",
	"Annie",
	"Annika",
	"Anthony",
	"Anton",
	"Antonio",
	"April",
	"Archer",
	"Archie",
	"Arlene",
	"Arnie",
	"Arnold",
	"Art",
	"Arthur",
	"Asaf",
	"Ashley",
	"Astrid",
	"Aubrey",
	"Austin",
	"Austine",
	"Avon",
	"Avril",
	"Axel",
	"Aziz",
	"Bailey",
	"Barbara",
	"Barney",
	"Barry",
	"Bart",
	"Barton",
	"Baxter",
	"Beck",
	"Becket",
	"Becky",
	"Belinda",
	"Bella",
	"Belle",
	"Ben",
	"Benjamin",
	"Benny",
	"Bernadette",
	"Bernard",
	"Bernice",
	"Bess",
	"Beth",
	"Bethany",
	"Bette",
	"Betty",
	"Bernard",
	"Bernardette",
	"Bernice",
	"Berty",
	"Bev",
	"Beverly",
	"Beverley",
	"Bianca",
	"Bill",
	"Billie",
	"Billy",
	"Bjorn",
	"Blaire",
	"Blake",
	"Blanche",
	"Bo",
	"Bob",
	"Bobbie",
	"Bobby",
	"Bonnie",
	"Boris",
	"Brad",
	"Bradley",
	"Brady",
	"Brandi",
	"Brandon",
	"Brandy",
	"Brenda",
	"Brendan",
	"Brendon",
	"Brent",
	"Brett",
	"Brian",
	"Bridgit",
	"Brigitte",
	"Britney",
	"Bruce",
	"Bruno",
	"Brutus",
	"Bryan",
	"Buck",
	"Bucky",
	"Bug",
	"Burton",
	"Byron",
	"Cailin",
	"Caitlyn",
	"Cal",
	"Caley",
	"Callum",
	"Calvin",
	"Cameron",
	"Camille",
	"Campbell",
	"Candy",
	"Carl",
	"Carla",
	"Carlene",
	"Carlos",
	"Carmela",
	"Carmen",
	"Carol",
	"Carole",
	"Caroline",
	"Carolyn",
	"Carrie",
	"Casey",
	"Cassandra",
	"Cassey",
	"Cassie",
	"Catherina",
	"Catherine",
	"Cathy",
	"Caz",
	"Cecelia",
	"Cecil",
	"Cecille",
	"Ceilidh",
	"Celeste",
	"Chad",
	"Charlene",
	"Charles",
	"Charlie",
	"Charlotte",
	"Chelsea",
	"Cher",
	"Cheri",
	"Cheryll",
	"Chip",
	"Chloe",
	"Chris",
	"Christel",
	"Christian",
	"Christie",
	"Christina",
	"Christine",
	"Christopher",
	"Chuck",
	"Cindy",
	"Clark",
	"Clair",
	"Claire",
	"Clara",
	"Clarabell",
	"Claude",
	"Claudette",
	"Claudia",
	"Clayton",
	"Cliff",
	"Clifford",
	"Clint",
	"Clive",
	"Clyde",
	"Codey",
	"Cody",
	"Colin",
	"Colleen",
	"Connie",
	"Coral",
	"Corina",
	"Craig",
	"Curtis",
	"Cynthia",
	"Cyril",
	"Darby",
	"Daisy",
	"Dale",
	"Damien",
	"Damon",
	"Dan",
	"Dana",
	"Daniel",
	"Danielle",
	"Danni",
	"Danny",
	"Daphne",
	"Darla",
	"Darlene",
	"Darrell",
	"Darren",
	"Darryl",
	"Dave",
	"David",
	"Davie",
	"Davis",
	"Dawn",
	"Dean",
	"Debbie",
	"Debby",
	"Deborah",
	"Debra",
	"Debs",
	"Deidre",
	"Delores",
	"Denise",
	"Dennis",
	"Denzel",
	"Derek",
	"Desmond",
	"Diana",
	"Diane",
	"Dianna",
	"Dick",
	"Dillon",
	"Dina",
	"Dominic",
	"Dominik",
	"Don",
	"Donald",
	"Donna",
	"Donovan",
	"Doreen",
	"Doris",
	"Dorothy",
	"Doug",
	"Dougal",
	"Douglas",
	"Doyle",
	"Drew",
	"Duane",
	"Dudley",
	"Duncan",
	"Dwight",
	"Dylan",
	"Earl",
	"Ed",
	"Eddie",
	"Edgar",
	"Edith",
	"Edmond",
	"Edward",
	"Edwin",
	"Edwina",
	"Eileen",
	"Elaine",
	"Elina",
	"Elisa",
	"Elisabeth",
	"Eliza",
	"Elizabeth",
	"Ella",
	"Ellen",
	"Elmer",
	"Elsie",
	"Emile",
	"Emilio",
	"Emily",
	"Emma",
	"Emmett",
	"Enrique",
	"Eric",
	"Erica",
	"Ericka",
	"Erik",
	"Erika",
	"Erin",
	"Erinn",
	"Ernest",
	"Esmeralda",
	"Esta",
	"Estella",
	"Esther",
	"Ethan",
	"Eugene",
	"Eva",
	"Evan",
	"Eve",
	"Evelyn",
	"Everett",
	"Felix",
	"Fabio",
	"Falicia",
	"Farah",
	"Felicity",
	"Fernando",
	"Fergus",
	"Fidelia",
	"Finlay",
	"Fiona",
	"Fletcher",
	"Flora",
	"Florence",
	"Floyd",
	"Fly",
	"Frances",
	"Francesca",
	"Francis",
	"Francisco",
	"Frank",
	"Franklin",
	"Franky",
	"Fraser",
	"Fred",
	"Freda",
	"Freddy",
	"Fuzz",
	"Gabriel",
	"Gabriela",
	"Gail",
	"Garrett",
	"Garth",
	"Gary",
	"Gavin",
	"Gayle",
	"Gene",
	"Genevieve",
	"Geoff",
	"Geoffrey",
	"George",
	"Gerald",
	"Geraldine",
	"Gerard",
	"Geri",
	"Gerry",
	"Gilbert",
	"Gillian",
	"Gina",
	"Ginger",
	"Giuseppe",
	"Gladys",
	"Glen",
	"Glenda",
	"Glenn",
	"Gloria",
	"Glyne",
	"Goldie",
	"Gordon",
	"Grace",
	"Graeme",
	"Graham",
	"Grant",
	"Grayson",
	"Greg",
	"Gregor",
	"Gregory",
	"Gretchen",
	"Gus",
	"Guy",
	"Gwen",
	"Gwendoline",
	"Hadrian",
	"Hamish",
	"Hank",
	"Hannah",
	"Hans",
	"Harley",
	"Harold",
	"Harry",
	"Harvey",
	"Haseem",
	"Hayley",
	"Hazel",
	"Heather",
	"Hector",
	"Heidi",
	"Helen",
	"Helena",
	"Henri",
	"Henry",
	"Herbert",
	"Herbie",
	"Hermann",
	"Hilda",
	"Hollie",
	"Holly",
	"Homer",
	"Horace",
	"Howard",
	"Hugh",
	"Hugo",
	"Iain",
	"Ian",
	"Imani",
	"Imelda",
	"Imran",
	"Ingrid",
	"Irene",
	"Irma",
	"Irving",
	"Isaac",
	"Isabella",
	"Isabelle",
	"Ishan",
	"Isla",
	"Ivan",
	"Ivanna",
	"Ivy",
	"Izola",
	"Jack",
	"Jacque",
	"Jacqueline",
	"Jacqui",
	"Jake",
	"Jakob",
	"James",
	"Jacob",
	"Jan",
	"Janet",
	"Jane",
	"Janice",
	"Jason",
	"Jasper",
	"Jay",
	"Jayne",
	"Jean",
	"Jeanette",
	"Jeff",
	"Jeffrey",
	"Jennifer",
	"Jenny",
	"Jeremy",
	"Jerry",
	"Jesse",
	"Jessica",
	"Jessie",
	"Jessy",
	"Jill",
	"Jillian",
	"Jim",
	"Jimbo",
	"Jimmy",
	"Jo",
	"Joan",
	"Joann",
	"Joanne",
	"Jock",
	"Jodi",
	"Joe",
	"Joel",
	"Joelyn",
	"Joey",
	"Johan",
	"John",
	"Johnathan",
	"Johnnie",
	"Johnny",
	"Jolynn",
	"Jon",
	"Jonah",
	"Jonas",
	"Jonathan",
	"Joni",
	"Jonny",
	"Jordan",
	"Jorge",
	"Jose",
	"Joseph",
	"Josephine",
	"Josh",
	"Joshua",
	"Joyce",
	"Juan",
	"Juana",
	"Juanita",
	"Judge",
	"Judie",
	"Judith",
	"Judy",
	"Julia",
	"Julian",
	"Julie",
	"Juliette",
	"Julio",
	"Julius",
	"June",
	"Justin",
	"Kaley",
	"Kaitlyn",
	"Kandice",
	"Kara",
	"Kareen",
	"Karen",
	"Karl",
	"Karolyne",
	"Karri",
	"Kate",
	"Katelyn",
	"Katey",
	"Kathy",
	"Katherine",
	"Kathie",
	"Kathleen",
	"Kathryn",
	"Katie",
	"Katrina",
	"Katy",
	"Katya",
	"Kay",
	"Keiko",
	"Keith",
	"Kelly",
	"Kelsey",
	"Ken",
	"Kenneth",
	"Kenny",
	"Kerry",
	"Kev",
	"Kevin",
	"Kieran",
	"Kim",
	"Kimberly",
	"Kiriaki",
	"Kirk",
	"Klaus",
	"Kris",
	"Krista",
	"Kristian",
	"Kristy",
	"Kurt",
	"Kurtis",
	"Kyle",
	"Kylie",
	"Laila",
	"Lana",
	"Lance",
	"Larry",
	"Lasse",
	"Latisha",
	"Laura",
	"Lauren",
	"Lauryn",
	"Laurie",
	"Lawrence",
	"Leah",
	"Lee",
	"Leigh",
	"Len",
	"Lena",
	"Lenore",
	"Leo",
	"Leon",
	"Leonard",
	"Leonardo",
	"Leone",
	"Leroy",
	"Les",
	"Leslie",
	"Lesley",
	"Lester",
	"Lewis",
	"Liam",
	"Lillian",
	"Lilly",
	"Lily",
	"Linda",
	"Lindsay",
	"Lindsey",
	"Lisa",
	"Lita",
	"Logan",
	"Lone",
	"Loren",
	"Loretta",
	"Lori",
	"Lorraine",
	"Lottie",
	"Louis",
	"Louise",
	"Lowell",
	"Lucas",
	"Lucy",
	"Luis",
	"Luke",
	"Luther",
	"Lydia",
	"Lynn",
	"Lynne",
	"Lyssa",
	"Mabel",
	"Madeline",
	"Maggie",
	"Magnus",
	"Mahamed",
	"Malcolm",
	"Mandy",
	"Manuel",
	"Marc",
	"Marcela",
	"Marci",
	"Marcia",
	"Marco",
	"Marcus",
	"Marcy",
	"Margaret",
	"Margarita",
	"Maria",
	"Mariah",
	"Marian",
	"Marianna",
	"Marie",
	"Marilyn",
	"Marina",
	"Marion",
	"Marisa",
	"Marissa",
	"Marjorie",
	"Mark",
	"Markus",
	"Marlene",
	"Marlin",
	"Marlon",
	"Marshall",
	"Martha",
	"Martin",
	"Martyn",
	"Marvin",
	"Mary",
	"Mathew",
	"Matt",
	"Matthew",
	"Maude",
	"Maureen",
	"Maurice",
	"Mauricio",
	"Mavis",
	"Max",
	"Maxine",
	"May",
	"Megan",
	"Meghan",
	"Mel",
	"Melanie",
	"Melany",
	"Melinda",
	"Melissa",
	"Melody",
	"Melvin",
	"Mervin",
	"Mhairi",
	"Mia",
	"Michael",
	"Michelle",
	"Mick",
	"Mickey",
	"Miguel",
	"Mikael",
	"Mike",
	"Mikey",
	"Miki",
	"Mikko",
	"Mildred",
	"Millie",
	"Milly",
	"Milton",
	"Miranda",
	"Miriam",
	"Mirriam",
	"Mitchell",
	"Mo",
	"Molly",
	"Monica",
	"Monique",
	"Monty",
	"Morgan",
	"Morten",
	"Moses",
	"Morris",
	"Muriel",
	"Murphy",
	"Murray",
	"Mustafa",
	"Myles",
	"Myrissa",
	"Myrtle",
	"Nadine",
	"Nancy",
	"Nanette",
	"Naomi",
	"Natalia",
	"Natalie",
	"Natasha",
	"Nathan",
	"Nathaniel",
	"Neil",
	"Nellie",
	"Nelly",
	"Nelson",
	"Neville",
	"Nicholas",
	"Nichole",
	"Nick",
	"Nico",
	"Nicola",
	"Nicolas",
	"Nicole",
	"Nigel",
	"Nikia",
	"Nikki",
	"Nina",
	"Noah",
	"Noel",
	"Norma",
	"Norman",
	"Norris",
	"Norvall",
	"Olga",
	"Olive",
	"Oliver",
	"Ollie",
	"Omar",
	"Oona",
	"Orve",
	"Orville",
	"Oscar",
	"Otto",
	"Owen",
	"Paisley",
	"Pam",
	"Pamela",
	"Pandora",
	"Pat",
	"Patricia",
	"Patrick",
	"Patty",
	"Paul",
	"Paula",
	"Pauline",
	"Pedro",
	"Peggy",
	"Penelope",
	"Penny",
	"Perry",
	"Pete",
	"Peter",
	"Phil",
	"Philip",
	"Phillip",
	"Phyllis",
	"Polly",
	"Preston",
	"Qasim",
	"Quentin",
	"Quinn",
	"Rachel",
	"Rae",
	"Rafael",
	"Raj",
	"Raja",
	"Ralph",
	"Ramon",
	"Randal",
	"Rashid",
	"Raquel",
	"Raul",
	"Ray",
	"Raymond",
	"Raymondo",
	"Rebecca",
	"Reg",
	"Regina",
	"Reginald",
	"Reinhold",
	"Rene",
	"Reuben",
	"Rex",
	"Rhonda",
	"Richard",
	"Rick",
	"Ricky",
	"Rita",
	"Robb",
	"Robert",
	"Roberta",
	"Robin",
	"Robina",
	"Robyn",
	"Robynne",
	"Rock",
	"Rockie",
	"Rod",
	"Rodney",
	"Rodrigo",
	"Roland",
	"Rolf",
	"Romeo",
	"Ronald",
	"Ronan",
	"Ronnie",
	"Roger",
	"Rosalind",
	"Rosanna",
	"Rosanned",
	"Rose",
	"Rosemary",
	"Rosetta",
	"Rosie",
	"Ross",
	"Roxanne",
	"Roy",
	"Russell",
	"Rosty",
	"Ruben",
	"Ruby",
	"Ruth",
	"Ryan",
	"Sabrina",
	"Sadie",
	"Sally",
	"Sam",
	"Samantha",
	"Sammy",
	"Samuel",
	"Sandra",
	"Sandy",
	"Sara",
	"Sarah",
	"Sasha",
	"Saul",
	"Scot",
	"Scott",
	"Sean",
	"Sebastian",
	"Sergio",
	"Shakira",
	"Shannon",
	"Shari",
	"Sharnell",
	"Sharon",
	"Sharyn",
	"Shawn",
	"Shelby",
	"Shelley",
	"Sherene",
	"Sheri",
	"Sherman",
	"Sherry",
	"Shirley",
	"Sheryl",
	"Shivani",
	"Shona",
	"Sian",
	"Sid",
	"Sidney",
	"Simon",
	"Sindy",
	"Sinead",
	"Sofia",
	"Sonny",
	"Sonja",
	"Sonya",
	"Sophia",
	"Sophie",
	"Spencer",
	"Stacey",
	"Stan",
	"Stanley",
	"Stefan",
	"Stephen",
	"Stephanie",
	"Steve",
	"Steven",
	"Stewart",
	"Stuart",
	"Sue",
	"Suki",
	"Susan",
	"Susana",
	"Susanne",
	"Susie",
	"Suzanne",
	"Sven",
	"Sylvester",
	"Sylvia",
	"Tabatha",
	"Tamara",
	"Tammie",
	"Tamsin",
	"Tania",
	"Tanya",
	"Tara",
	"Taylor",
	"Ted",
	"Teresa",
	"Terrance",
	"Terry",
	"Tess",
	"Tessa",
	"Tex",
	"Thelma",
	"Theodore",
	"Theresa",
	"Thomas",
	"Tiffany",
	"Tiger",
	"Tiko",
	"Tillie",
	"Tim",
	"Timmy",
	"Timothy",
	"Tina",
	"Toby",
	"Todd",
	"Tom",
	"Tomaki",
	"Tommy",
	"Tonia",
	"Tonie",
	"Tony",
	"Tracy",
	"Travis",
	"Trevor",
	"Tricia",
	"Trixie",
	"Troy",
	"Tucker",
	"Tyler",
	"Tyson",
	"Ulysses",
	"Uri",
	"Val",
	"Valerie",
	"Vanessa",
	"Vani",
	"Vaughn",
	"Velma",
	"Vernon",
	"Veronica",
	"Vicki",
	"Vicky",
	"Victor",
	"Victoria",
	"Vijay",
	"Vince",
	"Vincent",
	"Vinnie",
	"Virginia",
	"Viv",
	"Vivian",
	"Viviene",
	"Wally",
	"Walt",
	"Walter",
	"Walton",
	"Wanda",
	"Warren",
	"Wayne",
	"Wendell",
	"Wendy",
	"Wes",
	"Wesley",
	"Whitney",
	"Will",
	"William",
	"Willie",
	"Willis",
	"Wilson",
	"Winston",
	"Wyatt",
	"Xavier",
	"Yasmin",
	"Yogi",
	"Ysabel",
	"Yvonne",
	"Zachary",
	"Zachery",
	"Zola"
};

#pragma endregion

char real_name_initials[] = {
	'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'R', 'S', 'T', 'W'
};

void format_string_part_from_raw(char **dest, const char *src, char **args);
void format_string_part(char **dest, rct_string_id format, char **args);

const char *get_string(rct_string_id id)
{
	return RCT2_ADDRESS(0x009BF2D4, const char*)[id];
}

void format_integer(char **dest, int value)
{
	int digit;
	char *dst = *dest;
	char *finish;
	char tmp;

	// Negative sign
	if (value < 0) {
		*dst++ = '-';
		value = -value;
	}

	*dest = dst;

	if (value == 0) {
		*dst++ = '0';
	} else {
		// Right to left
		while (value > 0) {
			digit = value % 10;
			value /= 10;

			*dst++ = '0' + digit;
		}
	}
	finish = dst;

	// Reverse string
	dst--;
	while (*dest < dst) {
		tmp = **dest;
		**dest = *dst;
		*dst = tmp;
		(*dest)++;
		dst--;
	}
	*dest = finish;
}

void format_comma_separated_integer(char **dest, int value)
{
	int digit, groupIndex;
	char *dst = *dest;
	char *finish;
	char tmp;

	// Negative sign
	if (value < 0) {
		*dst++ = '-';
		value = -value;
	}

	*dest = dst;

	if (value == 0) {
		*dst++ = '0';
	} else {
		// Groups of three digits, right to left
		groupIndex = 0;
		while (value > 0) {
			// Append group seperator
			if (groupIndex == 3) {
				groupIndex = 0;
				*dst++ = ',';
			}

			digit = value % 10;
			value /= 10;

			*dst++ = '0' + digit;
			groupIndex++;
		}
	}
	finish = dst;

	// Reverse string
	dst--;
	while (*dest < dst) {
		tmp = **dest;
		**dest = *dst;
		*dst = tmp;
		(*dest)++;
		dst--;
	}
	*dest = finish;
}

void format_comma_separated_fixed_2dp(char **dest, int value)
{
	int digit, groupIndex;
	char *dst = *dest;
	char *finish;
	char tmp;

	// Negative sign
	if (value < 0) {
		*dst++ = '-';
		value = -value;
	}

	*dest = dst;

	// Two decimal places
	digit = value % 10;
	value /= 10;
	*dst++ = '0' + digit;
	digit = value % 10;
	value /= 10;
	*dst++ = '0' + digit;
	*dst++ = '.';

	if (value == 0) {
		*dst++ = '0';
	} else {
		// Groups of three digits, right to left
		groupIndex = 0;
		while (value > 0) {
			// Append group seperator
			if (groupIndex == 3) {
				groupIndex = 0;
				*dst++ = ',';
			}

			digit = value % 10;
			value /= 10;

			*dst++ = '0' + digit;
			groupIndex++;
		}
	}
	finish = dst;

	// Reverse string
	dst--;
	while (*dest < dst) {
		tmp = **dest;
		**dest = *dst;
		*dst = tmp;
		(*dest)++;
		dst--;
	}
	*dest = finish;
}

void format_currency(char **dest, int value)
{
	int digit, groupIndex;
	char *dst = *dest;
	char *finish;
	char tmp;

	// Negative sign
	if (value < 0) {
		*dst++ = '-';
		value = -value;
	}

	// Currency symbol
	*dst++ = '£';

	*dest = dst;

	// value is in tens of pennies, so to get rid of
	// the pence, just divide by ten
	format_comma_separated_integer(dest, value/10);
}

void format_currency_2dp(char **dest, int value)
{
	int digit, groupIndex;
	char *dst = *dest;
	char *finish;
	char tmp;

	// Negative sign
	if (value < 0) {
		*dst++ = '-';
		value = -value;
	}

	// Currency symbol
	*dst++ = '£';

	*dest = dst;

	// value is in tens of pennies, so multiply by 10
	// to get in pounds and pence
	format_comma_separated_fixed_2dp(dest, value*10);
}

void format_string_code(unsigned char format_code, char **dest, char **args)
{
	int value;

	switch (format_code) {
	case FORMAT_COMMA32:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_comma_separated_integer(dest, value);
		break;
	case FORMAT_INT32:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_integer(dest, value);
		break;
	case FORMAT_COMMA2DP32:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_comma_separated_fixed_2dp(dest, value);
		break;
	case FORMAT_COMMA16:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		format_comma_separated_integer(dest, value);
		break;
	case FORMAT_UINT16:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_integer(dest, value);
		break;
	case FORMAT_CURRENCY2DP:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_currency_2dp(dest, value);
		break;
	case FORMAT_CURRENCY:
		// Pop argument
		value = *((sint32*)*args);
		*args += 4;

		format_currency(dest, value);
		break;
	case FORMAT_STRINGID:
	case FORMAT_STRINGID2:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		format_string_part(dest, value, args);
		(*dest)--;
		break;
	case FORMAT_STRING:
		// Pop argument
		value = *((uint32*)*args);
		*args += 4;

		strcpy(*dest, (char*)value);
		*dest += strlen(*dest);
		break;
	case FORMAT_MONTHYEAR:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		uint16 dateArgs[] = { date_get_month(value), date_get_year(value) + 1 };
		uint16 *dateArgs2 = dateArgs;
		char formatString[] = "?, Year ?";
		formatString[0] = FORMAT_MONTH;
		formatString[8] = FORMAT_COMMA16;
		format_string_part_from_raw(dest, formatString, (char**)&dateArgs2);
		(*dest)--;
		break;
	case FORMAT_MONTH:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		strcpy(*dest, get_string(STR_MONTH_MARCH + date_get_month(value)));
		*dest += strlen(*dest);
		break;
	case FORMAT_VELOCITY:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, uint8)) {
			format_comma_separated_integer(dest, mph_to_kmph(value));
			strcpy(*dest, "kmh");
			*dest += strlen(*dest);
		} else {
			format_comma_separated_integer(dest, value);
			strcpy(*dest, "mph");
			*dest += strlen(*dest);
		}
		break;
	case FORMAT_POP16:
		*args += 2;
		break;
	case FORMAT_PUSH16:
		*args -= 2;
		break;
	case FORMAT_DURATION:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		if (value / 60 > 0) {
			format_integer(dest, value / 60);
			strcpy(*dest, value / 60 == 1 ? "min:" : "mins:");
			*dest += strlen(*dest);
		}

		format_integer(dest, value % 60);
		strcpy(*dest, value % 60 == 1 ? "sec:" : "secs:");
		*dest += strlen(*dest);
		break;
	case FORMAT_REALTIME:
		// Pop argument
		value = *((uint16*)*args);
		*args += 2;

		if (value / 60 > 0) {
			format_integer(dest, value / 60);
			strcpy(*dest, value / 60 == 1 ? "hour:" : "hours:");
			*dest += strlen(*dest);
		}

		format_integer(dest, value % 60);
		strcpy(*dest, value % 60 == 1 ? "min:" : "mins:");
		*dest += strlen(*dest);
		break;
	case FORMAT_LENGTH:
		// Pop argument
		value = *((sint16*)*args);
		*args += 2;

		if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_METRIC, uint8)) {
			format_comma_separated_integer(dest, value);
			strcpy(*dest, "m");
			*dest += strlen(*dest);
		} else {
			format_comma_separated_integer(dest, metres_to_feet(value));
			strcpy(*dest, "ft");
			*dest += strlen(*dest);
		}
		break;
	case FORMAT_SPRITE:
		// Pop argument
		value = *((uint32*)*args);
		*args += 4;

		*(*dest)++ = 23;
		*((uint32*)(*dest)) = value;
		*dest += 4;
		break;
	}
}

void format_string_part_from_raw(char **dest, const char *src, char **args)
{
	unsigned char code;
	while (1) {
		code = *src++;
		if (code < ' ') {
			if (code == 0) {
				*(*dest)++ = code;
				break;
			} else if (code <= 4) {
				*(*dest)++ = code;
				*(*dest)++ = *src++;
			} else if (code <= 16) {
				*(*dest)++ = code;
			} else if (code <= 22) {
				*(*dest)++ = code;
				*(*dest)++ = *src++;
				*(*dest)++ = *src++;
			} else {
				*(*dest)++ = code;
				*(*dest)++ = *src++;
				*(*dest)++ = *src++;
				*(*dest)++ = *src++;
				*(*dest)++ = *src++;
			}
		} else if (code <= 'z') {
			*(*dest)++ = code;
		} else if (code < 142) {
			format_string_code(code, dest, args);
		} else {
			*(*dest)++ = code;
		}
	}
}

void format_string_part(char **dest, rct_string_id format, char **args)
{
	if (format < 0x8000) {
		// Language string
		format_string_part_from_raw(dest, get_string(format), args);
	} else if (format < 0x9000) {
		// Custom string
		format -= 0x8000;
		// args += (format & 0xC00) >> 9;
		format &= ~0xC00;
		strcpy(*dest, RCT2_ADDRESS(0x135A8F4 + (format * 32), char));
		*dest = strchr(*dest, 0) + 1;
	} else if (format < 0xE000) {
		// Real name
		format -= -0xA000;
		sprintf(*dest, "%s %c.",
			real_names[format % countof(real_names)],
			real_name_initials[(format >> 10) % countof(real_name_initials)]
		);
		*dest = strchr(*dest, 0) + 1;
	} else {
		// ?
		RCT2_CALLPROC_EBPSAFE(RCT2_ADDRESS(0x0095AFB8, uint32)[format]);
	}
}

/**
 * Writes a formatted string to a buffer.
 *  rct2: 0x006C2555
 * dest (edi)
 * format (ax)
 * args (ecx)
 */
void format_string(char *dest, rct_string_id format, void *args)
{
	// RCT2_CALLPROC_X(0x006C2555, format, 0, (int)args, 0, 0, (int)dest, 0);
	format_string_part(&dest, format, (char**)&args);
}

/**
 *  rct2: 0x006E37F7
 *  error  (eax)
 *  format (bx)
 */
void error_string_quit(int error, rct_string_id format){
	//RCT2_CALLPROC_X(0x006E37F7, error, format, 0, 0, 0, 0, 0);
	RCT2_GLOBAL(0x14241A0, uint32) = error;
	RCT2_GLOBAL(0x9E2DA0, uint32) = 1;

	char* error_string = RCT2_ADDRESS(0x1424080, char);
	void* args = RCT2_ADDRESS(0x13CE952, void);
	*error_string = 0;

	if (format != 0xFFFF){
		format_string(error_string, format, args);
	}
	RCT2_GLOBAL(0x9E2D9C, uint32) = 1;
	rct2_exit();
	rct2_endupdate();
}

void generate_string_file()
{
	FILE* f;
	uint8** str;
	uint8* c;
	int i;

	f = fopen("english.txt", "w");

	for (i = 0; i < 4442; i++) {
		str = ((uint8**)(0x009BF2D4 + (i * 4)));
		if (*str == (uint8*)0xFFFFFFFF)
			continue;
		c = *str;

		fprintf(f, "STR_%04d    :", i);
		while (*c != '\0') {
			switch (*c) {
			case 7:		fputs("{TINYFONT}", f);				break;
			case 8:		fputs("{BIGFONT}", f);				break;
			case 9:		fputs("{MEDIUMFONT}", f);			break;
			case 10:	fputs("{SMALLFONT}", f);			break;

			case 11:	fputs("{OUTLINE}", f);				break;

			case 34:	fputs("{ENDQUOTES}", f);			break;

			case 123:	fputs("{COMMA32}", f);				break;
			case 124:	fputs("{INT32}", f);				break;
			case 125:	fputs("{COMMA2DP32}", f);			break;
			case 126:	fputs("{COMMA16}", f);				break;
			case 127:	fputs("{UINT16}", f);				break;
			case 128:	fputs("{CURRENCY2DP}", f);			break;
			case 129:	fputs("{CURRENCY}", f);				break;
			case 130:	fputs("{STRINGID}", f);				break;
			case 131:	fputs("{STRINGID2}", f);			break;
			case 132:	fputs("{STRING}", f);				break;
			case 133:	fputs("{MONTHYEAR}", f);			break;
			case 134:	fputs("{MONTH}", f);				break;
			case 135:	fputs("{VELOCITY}", f);				break;
			case 136:	fputs("{POP16}", f);				break;
			case 137:	fputs("{PUSH16}", f);				break;
			case 138:	fputs("{DURATION}", f);				break;
			case 139:	fputs("{REALTIME}", f);				break;
			case 140:	fputs("{LENGTH}", f);				break;
			case 141:	fputs("{SPRITE}", f);				break;

			case 142:	fputs("{BLACK}", f);				break;
			case 143:	fputs("{GREY}", f);					break;
			case 144:	fputs("{WHITE}", f);				break;
			case 145:	fputs("{RED}", f);					break;
			case 146:	fputs("{GREEN}", f);				break;
			case 147:	fputs("{YELLOW}", f);				break;
			case 148:	fputs("{TOPAZ}", f);				break;
			case 149:	fputs("{CELADON}", f);				break;
			case 150:	fputs("{BABYBLUE}", f);				break;
			case 151:	fputs("{PALELAVENDER}", f);			break;
			case 152:	fputs("{PALEGOLD}", f);				break;
			case 153:	fputs("{LIGHTPINK}", f);			break;
			case 154:	fputs("{PEARLAQUA}", f);			break;
			case 155:	fputs("{PALESILVER}", f);			break;

			case 159:	fputs("{AMINUSCULE}", f);			break;
			case 160:	fputs("{UP}", f);					break;
			case 163:	fputs("{POUND}", f);				break;
			case 165:	fputs("{YEN}", f);					break;
			case 169:	fputs("{COPYRIGHT}", f);			break;
			case 170:	fputs("{DOWN}", f);					break;
			case 171:	fputs("{LEFTGUILLEMET}", f);		break;
			case 172:	fputs("{TICK}", f);					break;
			case 173:	fputs("{CROSS}", f);				break;
			case 175:	fputs("{RIGHT}", f);				break;
			case 176:	fputs("{DEGREE}", f);				break;
			case 178:	fputs("{SQUARED}", f);				break;
			case 180:	fputs("{OPENQUOTES}", f);			break;
			case 181:	fputs("{EURO}", f);					break;
			case 184:	fputs("{APPROX}", f);				break;
			case 185:	fputs("{POWERNEGATIVEONE}", f);		break;
			case 186:	fputs("{BULLET}", f);				break;
			case 187:	fputs("{RIGHTGUILLEMET}", f);		break;
			case 188:	fputs("{SMALLUP}", f);				break;
			case 189:	fputs("{SMALLDOWN}", f);			break;
			case 190:	fputs("{LEFT}", f);					break;
			case 191:	fputs("{INVERTEDQUESTION}", f);		break;
			default:
				if (*c < 32 || *c > 127)
					fprintf(f, "{%d}", *c);
				else
					fputc(*c, f);
				break;
			}

			c++;
		}
		fputc('\n', f);
	}

	fclose(f);
}

/**
*
*  rct2: 0x006C4209
*/
void reset_saved_strings() {
	for (int i = 0; i < 1024; i++) {
		RCT2_ADDRESS(0x135A8F4, uint8)[i * 32] = 0;
	}
}
