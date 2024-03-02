#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_VALUES 100


int main(int argc, char *argv[]) {

    char * benvenuto;
    int simpleSocket = 0;
    int simplePort = 0;
    int returnStatus = 0;
    char buffer[256] = "";
    char servermessage[256] = "";
    struct sockaddr_in simpleServer;
    int err = 0;
    int total_values = 0;
    bool print_all_values = false;
    char all_values[512] = "";
    char clientMessageCopy[512];
    int k = 0;

    /* controllo sul numero di argomenti */
    if (3 != argc) {
        fprintf(stderr, "Numero di argomenti errato\n");
        exit(1);
    }

    /* creo una streaming socket */
    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (simpleSocket == -1) {
        fprintf(stderr, "Impossibile creare la socket!\n");
        exit(1);
    }
    else {
        fprintf(stderr, "Socket creata!\n");
    }

    /* ottengo il numero di porta da linea di comando */
    simplePort = atoi(argv[2]);

    /* setup degli indirizzi */
    memset(&simpleServer, '\0', sizeof(simpleServer));
    simpleServer.sin_family = AF_INET;
    simpleServer.sin_addr.s_addr = inet_addr(argv[1]);
    simpleServer.sin_port = htons(simplePort);

    /* connect al giusto indirizzo e numero di porta */
    returnStatus = connect(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));

    if (returnStatus == 0) {
        fprintf(stderr, "La connessione ha avuto successo!\n");
    }
    else {
        fprintf(stderr, "Impossibile connettersi all'indirizzo!\n");
        close(simpleSocket);
        exit(1);
    }
    
    /* QUI PARTE L'ESECUZIONE DEL PROGRAMMA */

    /* ricevo il benvenuto dal server e lo stampo */
read(simpleSocket, servermessage, sizeof(servermessage));
if (strncmp(servermessage, "OK START", 8) == 0) {
    printf("%s\n", servermessage + 9);
} else {
    printf("%s\n", servermessage);
}
    
    /* pulisco il buffer */
    memset(buffer, 0, sizeof(buffer));
    memset(servermessage, 0, sizeof(servermessage));

    /* chiedo all'utente di inserire un comando e gestisco eventuali errori */
    while(!err && total_values <= MAX_VALUES) {
        int valid_input = 0;
do {
    printf("Inserisci dati: esempio 4 10 20 30 40 \n");
    fgets(buffer, sizeof(buffer), stdin);
    valid_input = 1;
    for (int i = 0; i < strlen(buffer); i++) {
        if (!isspace(buffer[i])) { // se il carattere non è uno spazio
            valid_input = 0; // l'input non è valido
            break;
        }
    }
} while (buffer[0] == '\0' || valid_input);

        char formatted_input[512] = "";
        int num_values = 0;
        char *token = strtok(buffer, " ");
        while(token != NULL) {
            strcat(formatted_input, token);
            strcat(formatted_input, " ");
            token = strtok(NULL, " ");
            num_values++;
        }
        formatted_input[strlen(formatted_input) - 1] = '\0'; // rimuovi lo spazio finale

        /* aggiungi i nuovi valori a quelli già inviati */
        strcat(all_values, formatted_input);
        strcat(all_values, " ");

        /* invia la stringa di numeri formattata al server */
        write(simpleSocket, formatted_input, strlen(formatted_input));
        memset(buffer, 0, sizeof(buffer));
        read(simpleSocket, servermessage, sizeof(servermessage));

        /* controlla se il server ha inviato un messaggio di errore */
        if(strncmp(servermessage, "ERR DATA", strlen("ERR DATA")) == 0) {
            printf("%s\n", servermessage); // stampa il messaggio di errore
            close(simpleSocket); // chiude la connessione
            return 0;
        }
        
        if(strncmp(servermessage, "ERR SYNTAX", strlen("ERR SYNTAX")) == 0) {
            printf("%s\n", servermessage); // stampa il messaggio di errore
            close(simpleSocket); // chiude la connessione
            return 0;
        }
        
        if(strncmp(servermessage, "OK DATA", strlen("OK DATA")) == 0) {
            int n = sscanf(servermessage, "OK DATA %d\n", &num_values);
            printf("%s\n", servermessage);
        }
         
        if(strncmp(servermessage, "OK STATS", strlen("OK STATS")) == 0) {
       	 int numValues;
        	double mean;
        	double variance;
        	int itemsRead = sscanf(servermessage, "OK STATS %d %lf %lf\n", &numValues, &mean, &variance);
    	 
    		printf("%s\n", servermessage);
    		close(simpleSocket); // chiude la connessione
        	return 0;
	}
	
	if(strncmp(servermessage, "ERR STATS", strlen("ERR STATS")) == 0) {	
    		printf("%s\n", servermessage);
    		close(simpleSocket); // chiude la connessione
        	return 0;
	}
      
}
        
        

    /* chiudo la connessione */
    close(simpleSocket);
    printf("Connessione col server chiusa.\n");
    
    if(total_values > MAX_VALUES) {
        fprintf(stderr, "Errore: il numero di valori inseriti supera la capacità del server.\n");
        exit(1);
    }

    return 0;
}
