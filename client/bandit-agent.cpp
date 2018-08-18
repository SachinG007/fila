#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <vector>
#include <random>
#include <string>
#include <chrono>
#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"
#include "function.h"

// #include "function.h"

#define MAXHOSTNAME 256

using namespace std;

float cum_rewards[25] = {0};
int total_pulls[25] = {0};
float bandit_probs[25] = {0};
float ucb[50] = {0};
int tsuccess[50] ={0};
int tfails[50] = {0};
int ArmToPull = 0;
float beta_gen[50] = {0};

// C function to find maximum in arr[] of size n
int largest(float arr[], int n)
{
    // Initialize maximum element
    float max = arr[0];
    int max_index = 0;
 
    // Traverse array elements from second and
    // compare every element with current max  
    for (int i = 0; i < n; i++){
        if (arr[i] > max){
            max = arr[i];
            max_index = i;
          }
    }

    return max_index;
}

int random(int x, int y, double p)
{       
        // Generate a number from 1 to 100
        double r = rand() % 100 + 1;     // v2 in the range 1 to 100  
        r = r/100;    
        // r is smaller than px with probability px/100
        if (r <= p)
            return x;
 
        else
            return y;
}

int epsilon_greedy(int pulls, float reward, int numArr, double epsilon)
{

    cout << "eps"<<endl;
    //to be done only the first time
    if(pulls == 0){
        for(int i=0;i<numArr;i++){
            cum_rewards[i] = 0;
            total_pulls[i] = 0;
            bandit_probs[i] = 0;
        }
    }

    if(pulls!=0){
        cum_rewards[ArmToPull] = cum_rewards[ArmToPull] + reward;
        total_pulls[ArmToPull] = total_pulls[ArmToPull] + 1;
        bandit_probs[ArmToPull] = cum_rewards[ArmToPull]/total_pulls[ArmToPull];
    }


    int exploit = random(0,1,epsilon);

    // if(pulls<20){
      cout<<bandit_probs[0]<<endl;
      cout<<bandit_probs[1]<<endl;
      cout<<bandit_probs[2]<<endl;
      cout<<bandit_probs[3]<<endl;
      cout<<bandit_probs[4]<<endl;
      cout<<bandit_probs[5]<<endl;
      cout<<bandit_probs[6]<<endl;
    // }


    if(exploit==0){
        //do explore
        cout<<"explore"<<endl;
        ArmToPull = rand() % numArr ;
        return ArmToPull;
    }

    else {
        //exploit
        cout<<"exploit"<<endl;
        ArmToPull = largest(bandit_probs,numArr);
        return ArmToPull;
    }


    // return 2;
}


int UCB(int pulls, float reward, int numArms){

    cout<<"UCB"<<endl;
    //to be done only the first time
    if(pulls == 0){
        cout<<"cleaning"<<endl;
        for(int i=0;i<numArms;i++){
            cum_rewards[i] = 0;
            total_pulls[i] = 0;
            bandit_probs[i] = 0;
            ucb[i] = 0;
        }

        ArmToPull =0;
        return ArmToPull;
    }

    if(pulls < numArms){

        cum_rewards[ArmToPull] = reward;
        total_pulls[ArmToPull] = total_pulls[ArmToPull]+ 1;
        bandit_probs[ArmToPull] =  cum_rewards[ArmToPull]/total_pulls[ArmToPull];
        ArmToPull = pulls;
        return ArmToPull;
    }


    if(pulls >= numArms){
        cum_rewards[ArmToPull] = cum_rewards[ArmToPull] + reward;
        total_pulls[ArmToPull] = total_pulls[ArmToPull] + 1;
        bandit_probs[ArmToPull] = cum_rewards[ArmToPull]/total_pulls[ArmToPull];

        for(int i=0;i<numArms;i++){
           ucb[i] = bandit_probs[i] + sqrt( (2/total_pulls[i]) * log(1+pulls) );
         }

        ArmToPull = largest(ucb,numArms);
        return ArmToPull;
    }

}



  int Thompson(int pulls,float reward, int numArms)
  {   
    if(pulls==0){
        cout<<"cleaning"<<endl;
        for(int i=0;i<numArms;i++){
          tsuccess[i]=0;
          tfails[i]=0;
        }
        ArmToPull = 0;
    }

    else if(pulls!=0){
        
        if(reward<0.5){
          tfails[ArmToPull] = tfails[ArmToPull] + 1;
        }
        else{
          tsuccess[ArmToPull] = tsuccess[ArmToPull] + 1;
        }
    }

    for(int i=0;i<numArms;i++){
        
        // construct a trivial random generator engine from a time-based seed:
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator (seed);
        std::gamma_distribution<double> distribution1 (tsuccess[i] + 1,1);
        std::gamma_distribution<double> distribution2 (tfails[i] + 1,1);
        float x = distribution1(generator);
        float y = distribution2(generator);

        beta_gen[i] = x/(x+y);
    }

    ArmToPull = largest(beta_gen,numArms);
    return ArmToPull;

  }


void options(){

  cout << "Usage:\n";
  cout << "bandit-agent\n"; 
  cout << "\t[--numArms numArms]\n";
  cout << "\t[--randomSeed randomSeed]\n";
  cout << "\t[--horizon horizon]\n";
  cout << "\t[--hostname hostname]\n";
  cout << "\t[--port port]\n";
  cout << "\t[--algorithm algorithm]\n";
  cout << "\t[--epsilon epsilon]\n";

}



/*
  Read command line arguments, and set the ones that are passed (the others remain default.)
*/
bool setRunParameters(int argc, char *argv[], int &numArms, int &randomSeed, unsigned long int &horizon, string &hostname, int &port, string &algorithm, double &epsilon){

  int ctr = 1;
  while(ctr < argc){

    //cout << string(argv[ctr]) << "\n";

    if(string(argv[ctr]) == "--help"){
      return false;//This should print options and exit.
    }
    else if(string(argv[ctr]) == "--numArms"){
      if(ctr == (argc - 1)){
	return false;
      }
      numArms = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else if(string(argv[ctr]) == "--randomSeed"){
      if(ctr == (argc - 1)){
	return false;
      }
      randomSeed = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else if(string(argv[ctr]) == "--horizon"){
      if(ctr == (argc - 1)){
	return false;
      }
      horizon = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else if(string(argv[ctr]) == "--hostname"){
      if(ctr == (argc - 1)){
	return false;
      }
      hostname = string(argv[ctr + 1]);
      ctr++;
    }
    else if(string(argv[ctr]) == "--port"){
      if(ctr == (argc - 1)){
	return false;
      }
      port = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else if(string(argv[ctr]) == "--algorithm"){
      if(ctr == (argc - 1)){
  return false;
      }
      algorithm = string(argv[ctr + 1]);
      ctr++;
    }
     else if(string(argv[ctr]) == "--epsilon"){
      if(ctr == (argc - 1)){
  return false;
      }
      epsilon = atof(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else{
      return false;
    }

    ctr++;
  }

  return true;
}

/* ============================================================================= */
/* Write your algorithms here */
int sampleArm(string algorithm, double epsilon, int pulls, float reward, int numArms){
  if(algorithm.compare("rr") == 0){
    return(pulls % numArms);
  }
  else if(algorithm.compare("epsilon-greedy") == 0){
    // return(pulls % numArms);
    return(epsilon_greedy(pulls,reward,numArms,epsilon));
  }

  else if(algorithm.compare("UCB") == 0){
    return(UCB(pulls,reward,numArms));
  }
  else if(algorithm.compare("KL-UCB") == 0){
    return(pulls % numArms);
  }
  else if(algorithm.compare("Thompson-Sampling") == 0){
    return(Thompson(pulls,reward,numArms));
  }
  else{
    return -1;
  }
}

/* ============================================================================= */


int main(int argc, char *argv[]){
  // Run Parameter defaults.
  int numArms = 5;
  int randomSeed = time(0);
  unsigned long int horizon = 200;
  string hostname = "localhost";
  int port = 5000;
  string algorithm="random";
  double epsilon=0.1;

  //Set from command line, if any.
  if(!(setRunParameters(argc, argv, numArms, randomSeed, horizon, hostname, port, algorithm, epsilon))){
    //Error parsing command line.
    options();
    return 1;
  }


  setRunParameters(argc, argv, numArms, randomSeed, horizon, hostname, port, algorithm, epsilon);

  struct sockaddr_in remoteSocketInfo;
  struct hostent *hPtr;
  int socketHandle;

  bzero(&remoteSocketInfo, sizeof(sockaddr_in));
  
  if((hPtr = gethostbyname((char*)(hostname.c_str()))) == NULL){
    cerr << "System DNS name resolution not configured properly." << "\n";
    cerr << "Error number: " << ECONNREFUSED << "\n";
    exit(EXIT_FAILURE);
  }

  if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    close(socketHandle);
    exit(EXIT_FAILURE);
  }

  memcpy((char *)&remoteSocketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
  remoteSocketInfo.sin_family = AF_INET;
  remoteSocketInfo.sin_port = htons((u_short)port);

  if(connect(socketHandle, (struct sockaddr *)&remoteSocketInfo, sizeof(sockaddr_in)) < 0){
    //code added
    cout<<"connection problem"<<".\n";
    close(socketHandle);
    exit(EXIT_FAILURE);
  }


  char sendBuf[256];
  char recvBuf[256];  

  float reward = 0;
  unsigned long int pulls=0;
  int armToPull = sampleArm(algorithm, epsilon, pulls, reward, numArms);
  
  sprintf(sendBuf, "%d", armToPull);

  cout << "Sending action " << armToPull << endl;
  while(send(socketHandle, sendBuf, strlen(sendBuf)+1, MSG_NOSIGNAL) >= 0){

    char temp;
    recv(socketHandle, recvBuf, 256, 0);
    sscanf(recvBuf, "%f %c %lu", &reward, &temp, &pulls);
    cout << "Received reward " << reward << ".\n";
    cout<<"Num of  pulls "<<pulls<<".\n";


    armToPull = sampleArm(algorithm, epsilon, pulls, reward, numArms);

    sprintf(sendBuf, "%d", armToPull);
    cout << "Sending action " << armToPull << ".\n";


  }
  
  close(socketHandle);

  cout << "Terminating.\n";

  return 0;
}
          
