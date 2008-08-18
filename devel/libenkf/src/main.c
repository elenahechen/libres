#include <enkf_fs.h>
#include <enkf_main.h>
#include <enkf_config.h>
#include <enkf_site_config.h>
#include <util.h>
#include <basic_queue_driver.h>
#include <plain_driver.h>
#include <plain_driver_parameter.h>
#include <config.h>
#include <hash.h>
#include <fs_index.h>
#include <enkf_types.h>
#include <string.h>
#include <local_driver.h>
#include <lsf_driver.h>
#include <signal.h>
#include <ext_joblist.h>
#include <enkf_sched.h>
#include <stringlist.h>
#include <enkf_ui_main.h>


void install_SIGNALS(void) {
  signal(SIGSEGV , util_abort_signal);
}


void text_splash() {
  int i;
  {
#include "helge_lund.inc"
    printf("\n\n");
    for (i = 0; i < SPLASH_LENGTH; i++)
      printf("%s\n" , splash_text[i]);
    printf("\n\n");

    sleep(1);
#undef SPLASH_LENGTH
  }
}

void enkf_welcome() {
  printf("\n");
  printf("svn version......: %s \n",SVN_VERSION);
  printf("Compile time.....: %s \n",COMPILE_TIME_STAMP);
  printf("\n");
}


void enkf_usage() {
  printf("\n");
  printf(" *********************************************************************\n");
  printf(" **                       E n K F                                   **\n");
  printf(" **                                                                 **\n");
  printf(" **-----------------------------------------------------------------**\n");
  printf(" ** You have sucessfully started the EnKF program developed at      **\n");
  printf(" ** StatoilHydro. Before you can actually start using the program,  **\n");
  printf(" ** you must create a configuration file. When the configuration    **\n");
  printf(" ** file has been created, you can start the enkf application with: **\n");
  printf(" **                                                                 **\n");
  printf(" **   bash> enkf config_file                                        **\n");
  printf(" **                                                                 **\n");
  printf(" ** Instructions on how to create the configuration file can be     **\n");
  printf(" ** found at: http://sdp.statoil.no/wiki/index.php/Res:Setup-EnKF   **\n");
  printf(" *********************************************************************\n");
}


int main (int argc , char ** argv) {
  text_splash();
  enkf_welcome();
  install_SIGNALS();

  if (argc != 2) {
    enkf_usage();
    exit(1);
  } else {
    const char * site_config_file = SITE_CONFIG_FILE;  /* The variable SITE_CONFIG_FILE should be defined on compilation ... */
    const char * config_file      = argv[1];
    ext_joblist_type * joblist;
    job_queue_type   * job_queue;
    enkf_main_type   * enkf_main;
    enkf_site_config_type * site_config = enkf_site_config_bootstrap(site_config_file);
    joblist   = ext_joblist_alloc();

    enkf_config_type  * enkf_config              = enkf_config_fscanf_alloc(config_file , site_config , joblist , 1 , false , false , true);
    plain_driver_type * dynamic_analyzed 	 = plain_driver_alloc(enkf_config_get_ens_path(enkf_config) 	      , "%04d/mem%03d/Analyzed");
    plain_driver_type * dynamic_forecast 	 = plain_driver_alloc(enkf_config_get_ens_path(enkf_config) 	      , "%04d/mem%03d/Forecast");
    plain_driver_parameter_type * parameter      = plain_driver_parameter_alloc(enkf_config_get_ens_path(enkf_config) , "%04d/mem%03d/Parameter");
    plain_driver_type * eclipse_static           = plain_driver_alloc(enkf_config_get_ens_path(enkf_config)           , "%04d/mem%03d/Static");
    fs_index_type     * fs_index                 = fs_index_alloc(enkf_config_get_ens_path(enkf_config)               , "%04d/mem%03d/INDEX");
    enkf_fs_type      * fs = enkf_fs_alloc(fs_index , dynamic_analyzed, dynamic_forecast , eclipse_static , parameter);


    plain_driver_README( enkf_config_get_ens_path(enkf_config) );
    job_queue = enkf_config_alloc_job_queue(enkf_config , site_config);
    enkf_main = enkf_main_alloc(enkf_config , fs , job_queue , joblist);
    const enkf_sched_type * enkf_sched = enkf_sched_fscanf_alloc( enkf_config_get_enkf_sched_file(enkf_config) , enkf_main_get_sched_file(enkf_main) , joblist , enkf_config_get_forward_model(enkf_config));
    enkf_main_initialize_ensemble(enkf_main); 
    
    enkf_ui_main_menu(enkf_main , enkf_sched);
        
    job_queue_free(job_queue);
    enkf_main_free(enkf_main);
    enkf_site_config_free(site_config); /* Should probably be owned by enkf_main ?? */

    ext_joblist_free(joblist);
    enkf_fs_free(fs);  /* Takes the drivers as well */
  }
}
