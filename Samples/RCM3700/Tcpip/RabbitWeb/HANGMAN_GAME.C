/*
   Copyright (c) 2015, Digi International Inc.

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
/*******************************************************************************
   hangman_game.c

   Description
   ===========
   This demonstrates some RabbitWeb capabilities.
   This is the child's word guessing game "hangman."

   Instructions
   =============
   1. Set the IP address according to your network set up in TCP_CONFIG.LIB.
   2. Compile and run the program
   3. Open the hangman webpage in your web browser using
      http://xxx.xxx.xxx.xxx where xxx... is the IP address.
      Playing instructions are self explanatory
*******************************************************************************/

/*
  Pick the predefined TCP/IP configuration for this sample.  See
  LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
  configuration.
*/
#memmap xmem
#define TCPCONFIG      1  // 1 = Ethernet static static
#define USE_RABBITWEB  1  // must define this before #use'ing RabbitWeb

#use "dcrtcp.lib"
#use "http.lib"
#use "rand.lib"

// load images and web page to xmem at compile time
#ximport "hangman_game.c"  Csourcecode
#ximport "/pages/hang.zhtml" index_html
#ximport "/pages/hang0.gif"  hang0gif
#ximport "/pages/hang1.gif"  hang1gif
#ximport "/pages/hang2.gif"  hang2gif
#ximport "/pages/hang3.gif"  hang3gif
#ximport "/pages/hang4.gif"  hang4gif
#ximport "/pages/hang5.gif"  hang5gif
#ximport "/pages/hang6.gif"  hang6gif
#ximport "/pages/zsource.html"  Zsourcecode

// Create resource table
SSPEC_RESOURCETABLE_START
   SSPEC_RESOURCE_XMEMFILE("/", index_html),
   SSPEC_RESOURCE_XMEMFILE("/index.zhtml", index_html),
   SSPEC_RESOURCE_XMEMFILE("/zsource.html", Zsourcecode),
   SSPEC_RESOURCE_XMEMFILE("/hangman_game.c", Csourcecode),
   SSPEC_RESOURCE_XMEMFILE("/hang0.gif", hang0gif),
   SSPEC_RESOURCE_XMEMFILE("/hang1.gif", hang1gif),
   SSPEC_RESOURCE_XMEMFILE("/hang2.gif", hang2gif),
   SSPEC_RESOURCE_XMEMFILE("/hang3.gif", hang3gif),
   SSPEC_RESOURCE_XMEMFILE("/hang4.gif", hang4gif),
   SSPEC_RESOURCE_XMEMFILE("/hang5.gif", hang5gif),
   SSPEC_RESOURCE_XMEMFILE("/hang6.gif", hang6gif)
SSPEC_RESOURCETABLE_END

// Define file handlers
// the default for '/'  must be first
SSPEC_MIMETABLE_START
   SSPEC_MIME_FUNC(".zhtml", "text/html", zhtml_handler),
   SSPEC_MIME(".html", "text/html"),
   SSPEC_MIME(".gif", "image/gif"),
   SSPEC_MIME(".c", "text/html")
SSPEC_MIMETABLE_END

struct Word
{
   char* clue;
   char* answer;
};

const struct Word WordList[] = {
   {"action", "drool"}, {"action", "forecast"}, {"action", "forget"},
   {"action", "jump"}, {"action", "panic"}, {"action", "design"},
   {"action", "snore"}, {"action", "talk"}, {"action", "walk"},
   {"action", "debug"}, {"action", "repair"}, {"action", "crash"},
   {"action", "solder"}, {"action", "surf"}, {"action", "hack"},
   {"animal", "aardvark"}, {"animal", "kangaroo"}, {"animal", "beaver"},
   {"animal", "bison"}, {"animal", "cardinal"}, {"animal", "hare"},
   {"animal", "cat"}, {"animal", "donkey"}, {"animal", "elephant"},
   {"animal", "koala"}, {"animal", "otter"}, {"animal", "rabbit"},
   {"animal", "raccoon"}, {"animal", "rodent"}, {"animal", "shark"},
   {"animal", "sheep"}, {"animal", "squirrel"}, {"animal", "tiger"},
   {"creature", "elf"}, {"creature", "hobbit"}, {"creature", "gremlin"},
   {"creature", "troll"}, {"creature", "unicorn"}, {"creature", "vampire"},
   {"creature", "werewolf"}, {"creature", "dragon"}, {"creature", "sasquatch"},
   {"food", "apple"}, {"food", "butter"}, {"food", "lettuce"},
   {"food", "cashew"}, {"food", "celery"}, {"food", "cheese"},
   {"food", "chestnut"}, {"food", "cream"},  {"food", "donut"},
   {"food", "grape"}, {"food", "honey"}, {"food", "icecream"},
   {"food", "hamburger"}, {"food", "milk"}, {"food", "noodle"},
   {"food", "orange"}, {"food", "pasta"}, {"food", "pizza"},
   {"food", "potato"}, {"food", "carrot"}, {"food", "sugar"},
   {"food", "tomatoes"}, {"food", "turnip"}, {"food", "chow mein"},
   {"game", "poker"}, {"game", "golf"}, {"game", "pacman"}, {"game", "chess"},
   {"game", "hangman"}, {"game", "blackjack"}, {"game", "craps"},
   {"game", "scrabble"},
   {"character", "superman"}, {"character", "frosty"}, {"character", "grinch"},
   {"character", "gumby"}, {"character", "pointy haired boss"},
   {"character", "popeye"}, {"character", "snoopy"}, {"character", "waldo"},
   {"character", "dilbert"},
   {"software", "compiler"}, {"software", "operating system"},
   {"software", "function"}, {"software", "data structure"},
   {"software", "infinite loop"}, {"software", "assembly"},
   {"software", "break point"}, {"software", "algorithm"},
   {"software", "array"}, {"software", "stack"},
   {"hardware", "microprocessor"}, {"hardware", "computer"},
   {"hardware", "ethernet"}, {"hardware", "oscillator"},
   {"hardware", "capacitor"}, {"hardware", "resistor"},
   {"hardware", "inductor"}, {"hardware", "transistor"},
   {"hardware", "flash"}, {"hardware", "transformer"}, {"hardware", "inverter"},
   {"hardware", "printed circuit"}, {"hardware", "ram"},{"hardware", "battery"},
   {"person", "donald knuth"}, {"person", "albert einstein"},
   {"person", "joe montana"}, {"person", "john kennedy"},
   {"person", "alan turing"}, {"person", "bill gates"}, {"person", "geek"},
   {"country", "south korea"}, {"country", "chile"},  {"country", "mexico"},
   {"country", "france"}, {"country", "germany"}, {"country", "netherlands"},
   {"country", "italy"}, {"country", "denmark"}, {"country", "sweden"},
   {"country", "united kingdom"}, {"country", "switzerland"},
   {"country", "austria"}, {"country", "poland"}, {"country", "india"},
   {"country", "japan"}, {"country", "czech republic"},  {"country", "taiwan"},
   {"country", "israel"}, {"country", "turkey"}, {"country", "argentina"},
   {"country", "brazil"}, {"country", "canada"},  {"country", "venezuela"},
   {"country", "spain"}, {"country", "belgium"}, {"country", "norway"},
   {"country", "russia"}, {"country", "finland"}, {"country", "estonia"},
   {"country", "latvia"}, {"country", "ukraine"}, {"country", "albania"},
   {"country", "lithuania"}, {"country", "singapore"}, {"country", "malaysia"},
   {"country", "philippines"}, {"country", "thailand"}, {"country", "serbia"},
   {"country", "malta"}, {"country", "greece"}, {"country", "romania"},
   {"country", "hungary"}, {"country", "luxembourg"}, {"country","bulgaria"},
   {"country", "new zealand"}, {"country", "south africa"},
   {"country", "colombia"}, {"country", "el salvador"},{"country", "indonesia"},
   {"country", "portugal"},{"country", "united states"}, {"country", "ireland"},
   {"country", "australia"}, {"country", "uruguay"}, {"country", "peru"},
   {"country", "paraguay"}, {"country", "ecuador"}, {"country", "bolivia"},
   {"profession","astronaut"}, {"profession", "programmer"},
   {"profession", "engineer"}, {"profession", "lawyer"},
   {"profession", "mechanic"}, {"profession", "soldier"},
   {"profession", "system administrator"},
   {"company", "zworld"}, {"company", "rabbit semiconductor"},
   {"company", "imagine tools"}, {"company", "softools"},
   {"subject", "geography"},{"subject", "combinatorics"},{"subject", "algebra"},
   {"subject", "calculus"},  {"subject", "physics"}, {"subject", "history"},
   {"subject", "psychology"}, {"subject", "chemistry"},
   {"tool", "hammer"}, {"tool", "logic analyzer"}, {"tool", "soldering iron"},
   {"tool", "saw"}, {"tool", "drill"}, {"tool", "lathe"}, {"tool", "shovel"},
   {"tool", "pliers"}, {"tool", "screwdriver"}, {"tool", "wrench"},
   {"tool", "oscilloscope"}, {"tool", "voltmeter"}, {"tool", "wire stripper"}
};

// Define web variables
struct Word * wrd;
char guess[20], newGuess[40], prompt[80], hint[20];
int  numUsedGuesses, mode, currentGIF, character;
#web character
#web prompt
#web newGuess
#web numUsedGuesses
#web hint
#web currentGIF
#web mode

InitializeGame(){
    int wordIndex, i;

    // randomly select word
    rand16_init((int)MS_TIMER);
    wordIndex = rand16() % (sizeof(WordList) / sizeof(WordList[0]));
    wrd = &WordList[wordIndex];

    memset(guess,0,sizeof(guess)); // zero out guess

    // intialize strings
    for(i=0; i<strlen(wrd->answer); i++){
      if(wrd->answer[i]==' '){ // check for space between words
      	newGuess[2*i] = ' ';  // newGuess is string with blanks between letters
      	guess[i] = ' ';       // guess is string only, needs blank too
      }
      else{
      	newGuess[2*i] = '_';  // underline blank
      }
      newGuess[2*i+1] = ' ';   // space
    }
    newGuess[2*i] = 0;         // NULL terminate guess
    guess[strlen(wrd->answer)] = 0;
    strcpy(hint,wrd->clue);    // set hint
    currentGIF = 0;            // start with hang0.gif
    character = 0;
    numUsedGuesses = 0;
    strcpy(prompt,"Guess a letter of the above unknown word.");
}

main(){
   int correct, i;

   brdInit();

   //  sock_init initializes the TCP/IP stack.
   //  http_init initializes the web server.
   sock_init();
   http_init();

   tcp_reserveport(80);

   mode = -1;  // -1 = uninitialized

   while(1){

      if(mode== -1){
         InitializeGame();
         mode = 0;
      }

      http_handler();

      if(numUsedGuesses == 6){   // guesses used up
         mode = 1;
         numUsedGuesses = 0;
         strcpy(hint,wrd->answer);
      }

      if(character)  {  // new character entered
         correct = 0;   // assume incorrect guess

         // see if character is in word
         for(i = 0 ; i < strlen(wrd->answer) ; i++)  {
            if(wrd->answer[i] == (char)character) {
               guess[i] = newGuess[2*i] = (char)character;
               correct = 1;
            }
         }
         // see if word is guessed correctly
         if( !strcmpi(guess, wrd->answer) ){
            prompt[0] = 0;
            numUsedGuesses = 7; // 7 signals win
         }
         else if(correct == 0) {
            numUsedGuesses++;
            currentGIF++;       // use next GIF
            sprintf(prompt,"You have %d guesses left!", 6-numUsedGuesses);
         }
         character = 0;
      }
   }
}



