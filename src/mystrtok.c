#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mystrtok.h"
char* mystrtok( const char *phrase,const char *listeDeli)
{
    /*Cette fonction sectionne phrase en plusieurs morceaux,
    et retourne a chaque appel un de ces morceaux, jusqu'a retourner null*/


   static char copiePhrase2[1024]="";
   static char *copiePhrase;
   static int tCopiePhrase=0;
   static char *addInit;

   if(phrase)
   {

       /*Si phrase est different de NULL , alors, il faut decouper
       la phrase en plusieurs morceaux*/

       /*ON DEBUTE PAR quelque initialisation des variables statiques*/
       strcpy(copiePhrase2,phrase);
       //copiePhrase=phrase;
       copiePhrase=copiePhrase2;//pour ne pas alterer phrase
       tCopiePhrase=strlen(copiePhrase);
       /*Recuperer la position initiale de phrase, etant donné qu'elle changera*/
       addInit=copiePhrase;

       for(int i=0;i<tCopiePhrase;i++)
       {
           if(strchr(listeDeli,copiePhrase[i]))
           {
               /*Si le char situé a phrase+i est un delimiteur*/
               copiePhrase2[i]='\0';
                //phrase[i]='\0';
           }//fin if strchr(phrase[i],listeDeli)

       }//fin for(int i=0;i<sizeof(phras;i++)
       /*Retourner deja le premier bloc*/
       return copiePhrase2;
   }//fin if phrase
   else
   {
       /*Si phrase est un pointeur NULL, alors, User souhaite
       peutetre afficher la suite des blocs*/

        /*Selectionner le prochain blocs*/
        copiePhrase=copiePhrase+strlen(copiePhrase);

        /*Se deplacer jusqu'au prochain element non nul*/
        while(copiePhrase[0]=='\0' || strchr(listeDeli,copiePhrase[0])) copiePhrase++;

        /*Verifier si on est tjrs dans la zone d'adressage
        sinon, retourner NULL pour mentionner qu'il ya plus
        de chaines*/
        if(copiePhrase-addInit>=tCopiePhrase) return NULL;
        else
        {

            return copiePhrase;
        }
   }//fin if !phrase
}//fin char* mystrtok
