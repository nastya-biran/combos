/****************** CREAR DEPLOYMENT.XML CON EL NUMERO DE CLIENTES PASADO COMO ARGUMENTO ********************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "rand.h"
#include <errno.h>
#include <string.h>
#include "xbt/sysdep.h"

struct host_power {
	int cores;
	double core_power;
};

struct host_power get_host_power(FILE *fd){
	char *line = NULL;
	size_t len = 0;

	while(getline(&line, &len, fd) == -1){
		rewind(fd);	
	}
	
	struct host_power power;
	char* ptr = strtok(line, " ");
	power.cores = atoi(ptr);
	ptr = strtok(NULL, " ");
	power.core_power = atof(ptr);
	free(line);

	return power;
}

int generate_type(double* percentages, int* stats) {
	int overall_sum = 0;
	for (int i = 0; i < 4; i++) {
		overall_sum += percentages[i];
	}
	double rand = uniform_ab(0, overall_sum);
	double sum = 0;
	for (int i = 0; i < 4; i++) {
		sum += percentages[i];
		if (rand < sum) {
			stats[i]++;
			return i;
		}
	}
	stats[4]++;
	return -1;
}

int print_host(FILE* fd, int cluster_num, int host_num, double* percentage_by_type, int day, FILE* fd_traces, int* stats) {
	struct host_power power = get_host_power(fd_traces);
	int typ = generate_type(percentage_by_type, stats);
	for (int i = 0; i < power.cores; i++) {
		fprintf(fd, "   <process host=\"c%d%d\" function=\"client\"> ", cluster_num+1, host_num + i);
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%d\"/>  ", cluster_num); 				// <!-- Cluster number-->		
		fprintf(fd, "\n"); 		
		fprintf(fd, "        <argument value=\"%d\"/>  ", day); 				// <!-- Day when the host should join-->		
		fprintf(fd, "\n"); 	
		fprintf(fd, "        <argument value=\"%d\"/>  ", typ); 				// <!-- Host type-->		
		fprintf(fd, "\n"); 
		if(fd_traces != NULL){
			fprintf(fd, "        <argument value=\"%f\"/>  ", power.core_power); 		// <!-- Host power -->		
			fprintf(fd, "\n");
		} else {
			printf("Host power traces not provided");
			exit(1);
		}
		fprintf(fd, "   </process> ");
		fprintf(fd, "\n");
	}
	return power.cores;
}

int main(int argc, char *argv[]){
	
	int i, j, k, l = 0;
	int64_t index = 1;
	FILE *fd;
	char buffer[256];
	int n_projects, n_clusters, scheduling_server_number = 0;
	/* Usage 
	if (argc < 2) {
    		printf("Usage: %s n_clusters\n", argv[0]);
    		exit(1);
  	} */

	/* *********** DEPLOYMENT.XML *************/
	sprintf(buffer, "deployment.xml");
	fd= fopen(buffer, "w");
	if (fd == NULL){
    		printf("Error opening deployment file!\n");
    		exit(1);
	}

	seed(clock());

	/* BASICS */
	fprintf(fd, "<?xml version='1.0'?>\n");
	fprintf(fd, "<!DOCTYPE platform SYSTEM \"http://simgrid.gforge.inria.fr/simgrid.dtd\">\n");
	fprintf(fd, "<platform version=\"3\">\n");

	fprintf(fd, "   <process host=\"r0\" function=\"print_results\" />");
	fprintf(fd, "\n");

	n_projects = atoi(argv[index++]);
	int simulation_duration = atoi(argv[index++]);
	
	for(i=0; i<n_projects; i++){
		fprintf(fd, "   <process host=\"b%s\" function=\"init_database\"> ", argv[index]);
		fprintf(fd, "\n");	
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Numero del proyecto
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Nombre de proyecto
		fprintf(fd, "\n");	
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Tamanyo de respuesta - 64 KB
		fprintf(fd, "\n");
       		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Duracion de workunit (en flops)
		fprintf(fd, "\n");
       		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Percentage of input files generated locally
		fprintf(fd, "\n");
       		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Percentage of times a client must download new input files (they can't use previous ones)
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Quorum
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// target_nresults
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// max_error_results
		fprintf(fd, "\n");
       		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// max_total_results
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// max_success_results
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Deadline de workunit
		fprintf(fd, "\n");	
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Success percentage
		fprintf(fd, "\n");	
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Canonical percentage
		fprintf(fd, "\n");		
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Tamanyo de workunit - 360 KB
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Disk speed
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Number of data servers
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index++]);	// Input files replication
		fprintf(fd, "\n");
		fprintf(fd, "   </process> ");
		fprintf(fd, "\n");
		index-=18;	
		fprintf(fd, "   <process host=\"b%s\" function=\"work_generator\"> ", argv[index]);
		fprintf(fd, "\n");	
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index]);	// Numero del proyecto
		fprintf(fd, "\n");
		fprintf(fd, "   </process> ");
		fprintf(fd, "\n");
		fprintf(fd, "   <process host=\"b%s\" function=\"validator\" >", argv[index]);
		fprintf(fd, "\n");	
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index]);	// Numero del proyecto
		fprintf(fd, "\n");
		fprintf(fd, "   </process> ");
		fprintf(fd, "\n");
		fprintf(fd, "   <process host=\"b%s\" function=\"assimilator\" >", argv[index]);
		fprintf(fd, "\n");
		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index]);	// Numero del proyecto
		fprintf(fd, "\n");	
		fprintf(fd, "   </process> ");
		fprintf(fd, "\n");
		
		// Scheduling servers
		for(j=0; j<atoi(argv[index+18]); j++){	
			fprintf(fd, "   <process host=\"s%d%d\" function=\"scheduling_server_requests\"> ", i+1, j);
			fprintf(fd, "\n");
        		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index]);			// Numero del proyecto
			fprintf(fd, "\n");
			fprintf(fd, "           <argument value=\"%d\"/> ", scheduling_server_number);		// Numero del servidor
			fprintf(fd, "\n");
			fprintf(fd, "   </process> ");
			fprintf(fd, "\n");

			fprintf(fd, "   <process host=\"s%d%d\" function=\"scheduling_server_dispatcher\"> ", i+1, j);
			fprintf(fd, "\n");
        		fprintf(fd, "           <argument value=\"%s\"/> ", argv[index]);			// Numero del proyecto
			fprintf(fd, "\n");	
			fprintf(fd, "           <argument value=\"%d\"/> ", scheduling_server_number++);	// Numero del servidor
			fprintf(fd, "\n");
        		fprintf(fd, "   </process> ");
			fprintf(fd, "\n");
		}

		// Data servers
		for(j=0; j<atoi(argv[index+16]); j++, l++){
			fprintf(fd, "   <process host=\"d%d%d\" function=\"data_server_requests\"> ", i+1, j);		
			fprintf(fd, "\n");
			fprintf(fd, "           <argument value=\"%d\"/> ", l);
			fprintf(fd, "\n");
		        fprintf(fd, "   </process> ");
			fprintf(fd, "\n");
	
			fprintf(fd, "   <process host=\"d%d%d\" function=\"data_server_dispatcher\"> ", i+1, j);		
			fprintf(fd, "\n");
		        fprintf(fd, "           <argument value=\"%d\"/> ", l);
			fprintf(fd, "\n");
		        fprintf(fd, "           <argument value=\"%s\"/> ", argv[index]);
			fprintf(fd, "\n");
		        fprintf(fd, "   </process> ");
			fprintf(fd, "\n");
		}
		index+=19;		
	}

	n_clusters = atoi(argv[index++]);
	
	/* PRINT CLIENTS*/
	for(i=0; i<n_clusters;i++){
		int n_clients = atoi(argv[index++]);
		int new_hosts_distribution = atoi(argv[index++]);
		double new_hosts_param_a = atof(argv[index++]);
		double new_hosts_param_b = atof(argv[index++]);
		int att_projs = atoi(argv[index++]);
		char* traces_file = bprintf("../%s", argv[index++]);
		FILE *fd_traces = fopen(traces_file, "r");
		free(traces_file);
		double percantage_by_type[4];
		for (int j = 0; j< 4; j++) {
			percantage_by_type[j] = atof(argv[index++]);
		}

		// Process initial clients
		int stats[5] = {0};
		int overall_clients = 0;
		for(j=1; j<=n_clients; j++){
			overall_clients += print_host(fd, i, overall_clients + 1, percantage_by_type, 0, fd_traces, stats);
		}
		//Process new clients
		// Do not generate new reliable hosts 
		for(int j = 0; j < 5; j++) {
			fprintf(stderr,"%d ", stats[j]);
		}
		percantage_by_type[0] = 0;
		int new_hosts = 0;
		for (int day = 1; day <= simulation_duration / 24; day++)
		{
			int new_hosts_this_day = ran_distri(new_hosts_distribution, new_hosts_param_a, new_hosts_param_b);
			new_hosts += new_hosts_this_day;
			for (size_t k = 0; k < new_hosts_this_day; k++) {
				overall_clients += print_host(fd, i, overall_clients + 1, percantage_by_type, day, fd_traces, stats);
			}
		}
		fprintf(stderr, " new hosts %d\n", new_hosts);
		for(int j = 0; j < 5; j++) {
			fprintf(stderr,"%d ", stats[j]);
		}
		fprintf(stderr, "\n");
		fprintf(stderr, "%d\n", n_clients);
		printf("%d ", overall_clients);

		// READ LINE
		fprintf(fd, "   <process host=\"c%d%d\" function=\"client\"> ", i+1, 0);
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%d\"/>  ", i); 		// <!-- Cluster number-->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%d\"/>  ", overall_clients); 	// <!-- Number of clients -->
		fprintf(fd, "\n");
				fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- ConnectionInterval -->
		fprintf(fd, "\n");
				fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- SchedulingInterval -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- Max speed -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- Min speed -->
		fprintf(fd, "\n");               
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- Sp. random distribution -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- A argument -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- B argument -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- Av. random distribution -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- A argument -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- B argument -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- Nav. random distribution -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- A argument -->
		fprintf(fd, "\n");
		fprintf(fd, "        <argument value=\"%s\"/>  ", argv[index++]); // <!-- B argument -->
		struct host_power power = get_host_power(fd_traces);
		fprintf(fd, "\n");
		if(fd_traces != NULL){
			fprintf(fd, "        <argument value=\"%f\"/>  ", power.cores * power.core_power); 		// <!-- Host power -->		
			fprintf(fd, "\n");
		}	
		fprintf(fd, "        <argument value=\"%d\"/>", att_projs);	// <!-- Number of projects attached -->
		fprintf(fd, "\n");
		for(k=0; k<att_projs; k++){
					fprintf(fd, "        <argument value=\"Project%d\"/>", atoi(argv[index])+1);	// <!-- Project name ->	
			fprintf(fd, "\n");
			fprintf(fd, "        <argument value=\"%s\"/>", argv[index++]);		// <!-- Project number -->
			fprintf(fd, "\n");
					fprintf(fd, "        <argument value=\"%s\"/>", argv[index++]);		// <!-- Project priority -->
			fprintf(fd, "\n");
		}
		fprintf(fd, "   </process> ");
		fprintf(fd, "\n");
		if(fd_traces !=  NULL) fclose(fd_traces);	
	}

	/* END */
	fprintf(fd, "</platform>\n");

	fclose(fd);

	return 0;
}
