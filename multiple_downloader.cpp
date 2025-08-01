#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using namespace std;

/*-----------------------------------------------------------------------------------------------
 * Code originally written by Euibin Kim
 *
 * Last Modified: JULY 2025
 *
 * Code Description:
 * Function to download multiple CTX/HiRISE images
 *
 * Usage:
 *  ./multiple_downloader <image_type> <image_list> [directory]
 *
 *    <image_type> - Type of image to download
 *       ctx        --> CTX images
 *       hirise_edr --> HiRISE EDR images
 *       hirise_rdr --> HiRISE RDR images
 *
 *    <image_list> - a txt file containing the list of image_ids to download
 *    <directory>  - optional download directory (default: current directory)
-----------------------------------------------------------------------------------------------*/

#define NUM_PROCESS 5 // Number of processes to run in parallel

int main(int argc, char *argv[]) {

  // check command line inputs
  if (argc < 3 || argc > 4) {
    cout << "Usage: " << argv[0] << " <image_type> <image_list> [directory]"
         << endl;
    cout << " image_type options:" << endl;
    cout << "  ctx        - uses download_ctx.s" << endl;
    cout << "  hirise_edr - uses download_hirise_edr.s" << endl;
    cout << "  hirise_rdr - uses download_hirise_rdr.s " << endl;
    cout << " image_list: a text file containing list of image IDs (one per "
            "line)"
         << endl;
    cout << " directory: optional download directory (default: current "
            "directory)"
         << endl;
    return 1;
  }

  string image_type = argv[1];
  string image_ids_file = argv[2];

  // Get the current working directory
  char *original_cwd = getcwd(NULL, 0);
  string curr_dir = string(original_cwd);
  free(original_cwd);

  // Get download directory if provided
  string download_dir;
  if (argc == 4)
    download_dir = argv[3];
  else
    download_dir = curr_dir;

  // Choose download_script based on image_type (use absolute paths)
  string download_script;
  if (image_type == "ctx") {
    download_script = curr_dir + "/code/download_ctx.s";
  } else if (image_type == "hirise_edr") {
    download_script = curr_dir + "/code/download_hirise_edr.s";
  } else if (image_type == "hirise_rdr") {
    download_script = curr_dir + "/code/download_hirise_rdr.s";
  } else {
    cout << "Error: Invalid image_type '" << image_type << "'" << endl;
    cout << "Valid options: ctx, hirise_edr, hirise_rdr" << endl;
    return 1;
  }

  // Check if the download dscript exists
  struct stat script_stat;
  if (stat(download_script.c_str(), &script_stat) != 0) {
    cout << "Error: Download script not found: " << download_script << endl;
    return 1;
  }

  // Read image IDs from file (convert it to absolute path)
  string full_image_ids_file;
  if (image_ids_file[0] == '/') {
    full_image_ids_file = image_ids_file;
  } else {
    full_image_ids_file = curr_dir + "/" + image_ids_file;
  }

  // Read image IDs from file
  vector<string> image_ids;
  ifstream file(full_image_ids_file);
  if (!file.is_open()) {
    cout << "Error: Could not open file '" << image_ids_file << "'" << endl;
    return 1;
  }

  string line;
  while (getline(file, line)) {
    line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
    if (!line.empty()) {
      image_ids.push_back(line);
    }
  }
  file.close();

  if (image_ids.empty()) {
    cout << "Error: No image IDs found in file '" << image_ids_file << "'"
         << endl;
    return 1;
  }

  // Create download directory if DNE
  struct stat st = {0};
  if (stat(download_dir.c_str(), &st) == -1) {
    if (mkdir(download_dir.c_str(), 0777) != 0) {
      cout << "Error: Could not create directory '" << download_dir << "'"
           << endl;
      return 1;
    }
    cout << "Created download directory: " << download_dir << endl;
  }

  cout << "Found " << image_ids.size() << " image IDs to download." << endl;
  cout << "Using: " << download_script << endl;

  int NUM_IMAGES = image_ids.size();
  int next_image_index = 0; // index of next image to download
  int completed_count = 0;  // number of completed downloads

  // Map to track running processes: pid -> image_id
  map<pid_t, int> running_processes;
  pid_t pid;

  while (running_processes.size() < NUM_PROCESS &&
         next_image_index < NUM_IMAGES) {
    pid = fork();

    if (pid == 0) {
      // child process
      int curr = next_image_index;

      // Change to the download directory
      chdir(download_dir.c_str());

      execlp(download_script.c_str(), download_script.c_str(),
             image_ids[curr].c_str(), NULL);
      exit(0);
    } else if (pid > 0) {
      // parent process
      running_processes[pid] = next_image_index;
      cout << "Started downloading " << (next_image_index + 1) << "/"
           << NUM_IMAGES << " (PID: " << pid << ") - "
           << image_ids[next_image_index] << endl;
      next_image_index++;
    } else {
      // fork failed
      perror("fork failed");
      return 1;
    }
  }

  // as processes complete, start new ones
  while (completed_count < NUM_IMAGES) {
    int status;
    pid_t completed_pid = wait(&status);

    if (completed_pid > 0 &&
        running_processes.find(completed_pid) != running_processes.end()) {
      int completed_index = running_processes[completed_pid];
      completed_count++;

      running_processes.erase(completed_pid);

      // start a new download if there are more images to process
      if (next_image_index < NUM_IMAGES) {
        pid = fork();

        if (pid == 0) {
          // child process
          int curr = next_image_index;

          // Change to the download directory
          chdir(download_dir.c_str());

          execlp(download_script.c_str(), download_script.c_str(),
                 image_ids[curr].c_str(), NULL);
          exit(0);
        } else if (pid > 0) {
          // parent process
          running_processes[pid] = next_image_index;
          cout << "Started downloading " << (next_image_index + 1) << "/"
               << NUM_IMAGES << " (PID: " << pid << ") - "
               << image_ids[next_image_index] << endl;
          next_image_index++;
        } else {
          // fork failed
          perror("fork failed");
          return 1;
        }
      }
    }
  }

  cout << "All downloads completed!" << endl;
  return 0;
}
