19931101

Note: This is really old software, for the old PCC DOS compiler. Examine it carefully for suitability before use. It likely contains bugs.

VOTE 1.0 - November 1993

(*) means this item has changed in v1.1. See end of file for details.

This is a simple voting booth program, meant for use on a BBS system. Spread it if you like. Change whatever you want (but leave my name in there, eh?)

SETTING UP

To run it, you first must set up a file called VOTE.DAT(*). You can do that with any text editor. You'll need at least one vote in the file at all times, if you don't have one... anything can happen! (But it'll probably crash the program.)

The format of the VOTE.DAT file is as follows:

- question (as many lines as required - note that only 75 characters are read in at a time.)
- !<- Separator - must be first character. Rest of line is ignored. 
- This is for response number one. Again, as many lines as you need.
- !<- '!' is the separator again.
- 0/0 <-number of votes for this option/total votes. Rest of line must be blank
- Response option number two. The pattern repeats as necessary.
- !
- 0/0
- #<- vote separator. Next vote would go after this.
- $<- indicates that there are no more votes
- User #1 - user number, handle, whatever you pass to vote as "user" - CASE SENSITIVE!
- 0 - a string of binary flags, 1 for each vote. 0=not voted, 1=voted
- @<- end of file indicator. VERY IMPORTANT!!

You don't need to memorize the format, you could enter a file like this just to get things going, and then use the built-in functions after that to update the data file.

```
I will delete this later.
!
Ok.
!
0/0
#
$
no such user, no matter.
0
@
```

Currently the internal buffer allows for a 500 line data file. more later.

USING IT

The calling format is: VOTE mode user [path]

- mode: the mode you are calling VOTE in. The only valid modes(*) are N - for new votes, R - for read results, or S - for sysop. More later.

- user: the user ID you are using. This could be the handle, a user ID, or any similarly unique code(*)

- path: is optional. If included, it will use that path to find the VOTE.DAT file. If omitted, current directory is assumed. (*)

examples: 

- VOTE N FLIPPER - tells VOTE to see if there are any new questions for FLIPPER (case sensitive) to answer. If FLIPPER has not used VOTE before, he will be added, and have to do all the current votes.
- VOTE r FLIPPER - note that the mode switch is NOT case sensitive, only the user ID. Tells VOTE to enter voting review mode. The user ID is unneeded in review and sysop mode(*), but you need at least two arguments, or VOTE will dump the commandline summary at you
- VOTE n 015 c:\data\ - tells VOTE to do the new question procedure for user 015 (VOTE will treat the number 015 as a name), and to look for VOTE.DAT at the specified path (C:\DATA\VOTE.DAT). The trailing backslash is required.
- vote s bossman - enters sysop control mode.

You get the point, I'm sure.

MODE N - NEW
 
VOTE will first check if the user specified is in it's data file. If not, it will try to add the username to it's list (it will check and report if it is out of buffer space.)

Next it will scan the questions and the user's data, and bring up each voting question that he has not yet done. If it was a new user, that means all of them. The votes cannot be aborted (as requested!)

After each vote, the results to date of that vote will be displayed, showing the number of points per response, the total number of votes, and a percentage.

When all votes are done, VOTE will exit back to the calling program.

MODE R - REVIEW/RESULTS/REWHATEVER

No, I can't really decide for sure what to call this mode. Stop bugging me.

This mode will ask the user (doesn't matter what is passed as the username) what vote to start displaying results at. After each display, it will ask for the next result, or quit. If it shows the last vote, or quit is selected, it will return to the prompt asking which vote to start displaying at. If the response to that prompt is a blank <CR> (enter, return, whatever), the program will end.

MODE S - SYSOP

Again, it doesn't matter what is passed as a username here(*). VOTE will display a small menu when it starts:

- [A]dd question - lets you add a new vote to the list. You are told how many lines are free in the buffer. There must be at least 10 to go anywhere. If you overflow the buffer, might as well quit right away, cause the data file will not be saved (to avoid getting corrupted.)
- [D]elete question - lets you delete a vote, by number. The option is confirmed before it actually occurs. The users are also updated.
- [E]dit question(*)- in theory, should let you change the text of the question, any response, add a response, delete a response, or zeroize the results. In practice, I didn't implement it. If anyone really wants it, I'll think about it. (Assuming, of course, this program goes anywhere beyond Feral's BBS)
- [L]ist questions - will list the first line of each question and it's corresponding number, so you can know which one to delete (and potentially, some day, edit.)
- [Q]uit - quits back to the calling program

OTHER

At any time, control-S will pause the output, any key to resume. Control-C from local mode will stop the program (and not save the data file), dunno about remote. (Only works during printing or waiting for input, I think. it's part of the C compiler...)

Oh yeah. Only way to remove a user from the data file is to use a text editor. So if you need space, and you've had a lot of users who won't be back, you can delete them. Remember it's two lines per user (the name, and the flag line). Also ENSURE that the end-of-file marker ('@') remains intact, or the program will likely lock up looking for it.

I've included a sample VOTE.DAT file, you could simply use that, and delete my questions (replacing them with your own.)

The source is included, for anyone who wants to play around with it (and I'm positive you will, FF. My prompts are somewhat... mundane.) I compiled it with the PCC compiler and linker.

If there is any distribution, keep this doc and the source code with the program, eh? Thanks.

VOTE 1.1 - April 1994

The following things now work differently (they were marked with a '*' in the above text, so you'd know!)

There is a new mode, 'A'. This mode will allow anyone to add a single vote topic. It works the same as adding from the sysop menu.

Vote questions added by the program will now have an extra line which says "Contributed by: <username>". This means the minimum required lines to add a vote has been raised to 11. It also means the username passed in Sysop and Add modes is significant, because it is used when the question is added.

The path has been completly changed. If used, it is now a COMPLETE filename of a voting file. This allows multiple "vote.dat" files, because you can call them anything now. Unless you provide a full path, they must be in the same directory as VOTE.EXE. The default, if nothing is given, is still VOTE.DAT. Must be prefixed by '-' to distinguish it from part of a multi-word user ID! (EX: vote n Captain C -vote2.dat)

The old check for buffer overflow didn't really work... although I never ran into that problem. It checked, it set the overflow flag, then it (seems it would have) saved the data file anyway. There is now a check in the save routine to not save if the data buffer has been overflowed. *note* that you cannot undo an overflow by deleting a question! The file has been corrupted in memory, and you must exit VOTE and start again before making further changes. Maybe VOTE should dump you out, or prevent the overflow? It'd be hard to fix a half-finished question, though.

The [E]dit option has been removed from the sysop menu.

User ID may now be more than one word. The filepath, if used, must be prefixed with '-' with no spaces. See above under path for an example.
