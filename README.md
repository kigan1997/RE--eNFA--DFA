# RE->e-NFA->DFA


This project is about implementation of RE to eNFA to DFA in C


Compile:
gcc REtoNFAtoDFA.c -o out


./out


Enter symbol set:abc


Enter any Regular Expression:(a+E)*.b.c*


aE+*bc*..


Starting state: -->6


Final state: (13)


The NFA transition table is as follows:


	a	b	c	ep
	
q0:	q1,	-	-	-	

q1:	-	-	-	q5,	

q2:	-	-	-	q3,	

q3:	-	-	-	q5,	

q4:	-	-	-	q0,q2,	

q5:	-	-	-	q4,q7,	

q6<--:	-	-	-	q4,q7,	

q7:	-	q9,	-	-	

q9:	-	-	-	q10,q13,	

q10:	-	-	q11,	-	

q11:	-	-	-	q10,q13,	

(q13):	-	-	-	-	




The equivalent DFA transition table is as follows:

	a	b	c	

S0<--:	S1	S2	S3	

S1:	S1	S2	S3	

(S2):	S3	S3	S4	

S3(R):	S3	S3	S3	

(S4):	S3	S3	S4	

