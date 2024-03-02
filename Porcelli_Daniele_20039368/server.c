#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>




int countData(const char* str, char delimiter) {
    int count = 0;
    int length = strlen(str);

    for (int i = 0; i < length; i++) {
        if (str[i] == delimiter) {
            count++;
        }
    }

    // Aggiungi 1 per contare l'ultimo elemento dopo l'ultimo delimitatore
    count++;

    return count-1;
}

#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    int serverSocket, newSocket, portNum;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    int *arrayPointer = NULL;
    int numValues = 0;
    // dichiarazione della variabile per la copia del messaggio
    char clientMessageCopy[BUFFER_SIZE];

    // controllo argomenti
    if (argc < 2) {
        fprintf(stderr,"Errore: inserire il numero di porta.\n");
        exit(1);
    }

    // creazione della socket
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        fprintf(stderr,"Errore nella creazione della socket.\n");
        exit(1);
    }

    // setup degli indirizzi
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // bind della socket
    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        fprintf(stderr,"Errore nella bind della socket.\n");
        exit(1);
    }

    // listen sulla socket
    if (listen(serverSocket, 5) != 0) {
        fprintf(stderr,"Errore nella listen sulla socket.\n");
        exit(1);
    }

    while (true) {
        // accept della connessione
        addr_size = sizeof(serverStorage);
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        if (newSocket < 0) {
            fprintf(stderr,"Errore nell'accept della connessione.\n");
            exit(1);
        }

        // invio del messaggio di benvenuto al client
        char *welcomeMessage = "OK START Benvenuto al server! Digita valori nel formato: <Numero_dati> <dato1> <dato2> <datoN> e 0 se vuoi elaborare i dati\n";
        if (send(newSocket, welcomeMessage, strlen(welcomeMessage), 0) < 0) {
            fprintf(stderr,"Errore nell'invio del messaggio di benvenuto.\n");
            exit(1);
        }

        while (true) {
            bool syntax_error = false;
            char clientMessageCopy[BUFFER_SIZE] = "";

            // 1.0 - Riceve dal client
            char clientMessage[BUFFER_SIZE] = "";
            if (recv(newSocket, clientMessage, sizeof(clientMessage), 0) < 0) {
                fprintf(stderr,"Errore nella ricezione del messaggio dal client.\n");
                exit(1);
            }

            
             
	    strcpy(clientMessageCopy,clientMessage+1);
	  

   
           // Calcola il numero di dati presenti nella copia
           int sos = countData(clientMessageCopy, ' ');
    

            // Parse the first number from clientMessage

            int newValuesCount = 0;
            // Si da per scontato che ci siano solo interi

            /*
             * Se necessario, fai una copia di clientMessage, e controlla, che
             * siano tutti interi, e che num_values combaci
             * con le lunghezze dell'array
             */

            // 2.0 prendo il primo numero
            sscanf(clientMessage, "%d", &newValuesCount);
            

            if (newValuesCount > 0) {
                // Reallocate memory for the array
                int *tempPointer = (int*)realloc(arrayPointer, (numValues + newValuesCount) * sizeof(int));
                if (tempPointer == NULL) {
                    fprintf(stderr, "Errore nell'allocazione di memoria.\n");
                    exit(1);
                }
                arrayPointer = tempPointer;

                // Parse the additional numbers and store them in the array
                char* token = strtok(clientMessage, " ");
                int i = numValues;
                while (token != NULL && i < numValues + newValuesCount) {
                    token = strtok(NULL, " ");
                    if (token != NULL) {
                       if (!isdigit(*token)) {
        			// invio del messaggio di errore al client
                char errorMessage[BUFFER_SIZE] = "";
                sprintf(errorMessage, "ERR SYNTAX Hai inserito un carattere non valido");
                if (send(newSocket, errorMessage, strlen(errorMessage), 0) < 0) {
                    fprintf(stderr,"Errore nell'invio del messaggio di errore.\n");
                    exit(1);
                }
                // chiusura della connessione
                close(newSocket);
                break;
                free(arrayPointer);
                arrayPointer = NULL;
                numValues = 0;
        		 }
                        arrayPointer[i] = atoi(token);
                        i++;
                    }
                    
                    	
                }

                // Update the total count of values
                numValues += newValuesCount;
            }
           
           
             int numData;
             sscanf(clientMessage, "%d", &numData);
             
            
	     
    
    	     if (numData == 0) 
    	     {
	     		double sum = 0;
    			double mean = 0;
    			double variance = 0;
    		
       		for (int i = 0; i < numValues; i++) {
           			 sum += arrayPointer[i];
       		}
       		mean = sum / numValues;
        		
        		for (int i = 0; i < numValues; i++) {
            			variance += pow(arrayPointer[i] - mean, 2);
        		}
        		
        		variance = variance / numValues;
        	
        		if (numValues < 2 ) {
                  		char message[BUFFER_SIZE] = "";
                  		snprintf(message, BUFFER_SIZE, "ERR STATS Non posso calcolare la varianza di 1 campione\n");
                  		if (send(newSocket, message, strlen(message), 0) < 0) {
       			 fprintf(stderr,"Errore nell'invio del messaggio di errore.\n");
        			 exit(1);
   				 }
   				 free(arrayPointer);
   				 arrayPointer = NULL;
   				 numValues = 0;
   				  // chiusura della connessione
                close(newSocket);
                break;
               	} 
               	else {
                		 char message[BUFFER_SIZE] = "";
        snprintf(message, BUFFER_SIZE, "OK STATS %d %f %f\n", numValues, mean, variance);
         if (send(newSocket, message, strlen(message), 0) < 0) {
        fprintf(stderr,"Errore nell'invio del messaggio di OK STATS.\n");
        exit(1);
    				}
    				free(arrayPointer);
    				arrayPointer = NULL;
    				numValues = 0;
                		close(newSocket);
                		break;
           		 }
            } 
    
	 
	     	
           
            if (!isdigit(clientMessage[0]))
            {
            	// invio del messaggio di errore al client
                char errorMessage[BUFFER_SIZE] = "";
                sprintf(errorMessage, "ERR SYNTAX Il messaggio non e' sintatticamente corretto. Utilizzo corretto: <Numero_dati> <dato1> <dato2> <datoN>\n");
                if (send(newSocket, errorMessage, strlen(errorMessage), 0) < 0) {
                    fprintf(stderr,"Errore nell'invio del messaggio di errore.\n");
                    exit(1);
                }
                free(arrayPointer);
                arrayPointer = NULL;
                numValues = 0;
                // chiusura della connessione
                close(newSocket);
                break;
           }
           
           if ( numData != sos)
           {
          	// invio del messaggio di errore al client
                char errorMessage[BUFFER_SIZE] = "";
                sprintf(errorMessage, "ERR DATA Numero Dati non corrisponde con numero elementi");
                if (send(newSocket, errorMessage, strlen(errorMessage), 0) < 0) {
                    fprintf(stderr,"Errore nell'invio del messaggio di errore.\n");
                    exit(1);
                }
                // chiusura della connessione
                free(arrayPointer);
                arrayPointer = NULL;
                numValues = 0;
                close(newSocket);
                break;
           
           }
           
           
            if ( newValuesCount != 0)
            {
            // 3.0 costruzione del messaggio di risposta
            char responseMessage[BUFFER_SIZE] = "";
            sprintf(responseMessage, "OK DATA %d", newValuesCount);

            // invio del messaggio di risposta al client
            if (send(newSocket, responseMessage, strlen(responseMessage), 0) < 0) {
                fprintf(stderr,"Errore nell'invio del messaggio di conferma.\n");
                exit(1);
            }
           }
        }
    }
    return 0;
}
