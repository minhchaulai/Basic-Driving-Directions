//using namespace std;
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace web::json;                  // JSON library

//trims the string to remove leading and trailing white spaces. this is used when we're getting lines from the API's json results
std::string trim(const std::string& str,
                 const std::string& whitespace = " \t"){
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

//checks to see if the cities entered exist in the map, and if a path betweens them. returns three different cases
int Graph::foundinmap(std::string v1, std::string v2){
  bool found1=false;
  bool found2=false;
  bool foundp=false;
  for (int i=0; i<end; i++){
    if (vertices[i].city==v1){
      found1=true;
    }
  }
  for (int i=0; i<end; i++){
    if (vertices[i].city==v2){
      found2=true;
    }
  }
  if (found1==false){
    std::cout<<"First city not found!"<<std::endl;
    return 1;
  }
  if (found2==false){
    std::cout<<"Second city not found!"<<std::endl;
    return 1;
  }
  else if (found2==true and found1==true){
    for (int i=0; i<end; i++){
      if (vertices[i].city==v1){
        int e=vertices[i].adjacent.size();
        for (int y=0; y<e; y++){
          if (vertices[vertices[i].adjacent[y].index].city==v2){
            return 2;
          }
        }
      }
    }
  }
  return 3;
}

void Graph::insertPlace(std::string city){
  //checks to see if the city is already in the map, adds it if not. does NOT add paths, that's done manually
  bool found=false;
  for (int i=0; i<end; i++){
    if (vertices[i].city==city){
      found=true;
      std::cout<<"City already in map"<<std::endl;
      return;
    }
  }
  if (found==false){
    vertex hold(city);
    vertices[end]=hold;
    end++;
    std::cout<<city<<" added"<<std::endl;
  }
}

Graph::Graph(){
  //default constructor with 4 example cities and paths. These use precise numbers cause I pulled them off the Maps UI, but the program's API calls round to integer values
  end=0;
  insertPlace("Boulder CO");
  insertPlace("Denver CO");
  insertPlace("Fort Collins CO");
  insertPlace("Niwot CO");
  insertPath("Boulder CO", "Denver CO", 29.3, 47.0);
  insertPath("Boulder CO", "Niwot CO", 9.4, 20.0);
  insertPath("Fort Collins CO", "Denver CO", 64.9, 65.0);
  insertPath("Fort Collins CO", "Niwot CO", 36.8, 60.0);
};

Graph::~Graph(){}

void Graph::insertPath(std::string v1, std::string v2, float m, float t){
  //iterates through the array to find the first city, then iterate through the vector to see if second city is listed in adjacency vector
  for (int i=0; i<end; i++){
    if (vertices[i].city==v1){
      int e=vertices[i].adjacent.size();
      for (int y=0; y<e; y++){
        if (vertices[vertices[i].adjacent[y].index].city==v2){
          std::cout<<"Path already exists"<<std::endl;
          return;
        }
      }
    }
  }
  //if not, navigates to first city and inserts city into adjacency list, then vice versa
  for (int x=0; x<end; x++){
    if (vertices[x].city==v1){
      for (int y=0; y<end; y++){
        if (vertices[y].city==v2 and x!=y){
          vertex::path hold(y, m, t);
          vertices[x].adjacent.push_back(hold);
          vertex::path hold2(x, m, t);
          vertices[y].adjacent.push_back(hold2);
        }
      }
    }
  }
}

void Graph::printGraph(){
  //iterates through array and through each element's vector to print the city and what it's adjacent to
  for (int x=0; x<end; x++){
    std::cout<<vertices[x].city<<std::endl;
    int e=vertices[x].adjacent.size();
    for (int i=0; i<e; i++){
      std::cout<<"Adjacent to: "<<vertices[vertices[x].adjacent[i].index].city<<" by "<<vertices[x].adjacent[i].miles<<" miles and "<<vertices[x].adjacent[i].time<<" minutes."<<std::endl;
    }
  }
}

void Graph::findPath(std::string v1, std::string v2){
  //makes an API call to Google Maps API and uses the distance/duration data to make a path. the query builder is cool, you can basically add anything if it's drivable
  auto fileStream = std::make_shared<ostream>();
  // Open stream to output file.
  pplx::task<void> requestTask = fstream::open_ostream(U("results.json")).then([=](ostream outFile){
      *fileStream = outFile;
      // Create http_client to send the request.
      http_client client(U("https://maps.googleapis.com/maps/api/directions/"));
      // Build request URI and start the request.
      uri_builder builder(U("/json?"));
      builder.append_query(U("origin"), U(v1));
      builder.append_query(U("destination"), U(v2));
      builder.append_query(U("key"), U("AIzaSyDNXWl24ISjgrTj9ymN-NOHZjASA_9MtyU"));
      return client.request(methods::GET, builder.to_string());
  })
  .then([=](http_response response){
      printf("Received response status code:%u\n", response.status_code());
      // Write response body into the file.
      return response.body().read_to_end(fileStream->streambuf());
  })
  // Close the file stream.
  .then([=](size_t){
      return fileStream->close();
  });
  // Wait for all the outstanding I/O to complete and handle any exceptions
  try{
      requestTask.wait();
  }
  catch (const std::exception &e){
      printf("Error exception:%s\n", e.what());
  }
  std::fstream myfile;
  myfile.open("results.json");
  //open the JSON file to parse for data
  /*if (myfile.is_open()){
    std::cout<<"File opened"<<std::endl;
  }*/
  std::string line;
  int count=0;
  //first instance of this is the value we need, so we just getline until we find this
  while (line!="\"distance\" : {" and count<300){
    std::getline(myfile, line);
    //since the file is json, we need to strip the whitespaces for the comparison operator to work
    line=trim(line);
    count++;
  }
  std::getline(myfile, line);
  std::getline(myfile, line);
  line=trim(line);
  //using string stream to parse the line
  std::stringstream ss(line);
  std::string a, b, c;
  ss>>a;
  ss>>b;
  ss>>c;
  //c will always have the actual value, so we convert from string to float
  float miles=0;
  miles=std::stof(c);
  miles=miles/1609.34;
  miles=round(miles);
  std::getline(myfile, line);
  std::getline(myfile, line);
  std::getline(myfile, line);
  std::getline(myfile, line);
  std::stringstream ss2(line);
  ss2>>a;
  ss2>>b;
  ss2>>c;
  float time=0;
  time=std::stof(c);
  time=time/60;
  time=round(time);
  std::cout<<"Path added: "<<miles<<" miles and "<<time<<" minutes from "<<v1<<" to "<<v2<<std::endl;
  //now that we have the data we need, insert the path
  insertPath(v1, v2, miles, time);
  myfile.close();
  return;
}

void Graph::search(std::string v1){
  //searches the map to see the city and its adjacency list
  for (int x=0; x<end; x++){
    if (vertices[x].city==v1){
      std::cout<<"City found:"<<std::endl;
      std::cout<<vertices[x].city<<std::endl;
      int e=vertices[x].adjacent.size();
      for (int y=0; y<e; y++){
        std::cout<<"Adjacent to: "<<vertices[vertices[x].adjacent[y].index].city<<" by "<<vertices[x].adjacent[y].miles<<" miles and "<<vertices[x].adjacent[y].time<<" minutes."<<std::endl;
      }
      return;
    }
  }
  std::cout<<"City not found"<<std::endl;
}

void Graph::deletePlace(std::string v1){
  //check to see if the city is there to be deleted
  bool found=false;
  int dex;
  for (int x=0; x<end; x++){
    if (vertices[x].city==v1){
      dex=x+1;
      found=true;
    }
  }
  if (found==true){
    //iterates through the array and each element's vector
    for (int t=0; t<end; t++){
      int e=vertices[t].adjacent.size();
      for (std::vector<vertex::path>::iterator it=vertices[t].adjacent.begin(); it!=vertices[t].adjacent.end(); ++it){
        //erases from adjacency list if in there, so now no cities will have city being deleted in adjacency list
        if (vertices[it->index].city==v1){
          vertices[t].adjacent.erase(it);
          break;
        }
      }
    }
    //now remove from the array itself and reorganize the array to have no empty spots
    int a=dex;
    while (a<end){
      vertices[a-1]=vertices[a];
      a++;
    }
    end--;
    //since the array has been shifted, we must adjust the adjacency lists to make sure they're pointing to the right places. only
    //things to the right of the deleted element got removed, so indexes stored in adjacency larger than that get adjusted to the left by 1
    for (int t=0; t<end; t++){
      int d=vertices[t].adjacent.size();
      for (std::vector<vertex::path>::iterator it=vertices[t].adjacent.begin(); it!=vertices[t].adjacent.end(); ++it){
        if (it->index>dex-1){
          it->index--;
        }
      }
    }
    return;
  }
  else {
    std::cout<<"City not found"<<std::endl;
  }
}

void Graph::deletePath(std::string v1, std::string v2){
  int dex1;
  int dex2;
  //search for and hold indexes of the two cities
  for (int x=0; x<end; x++){
    if (vertices[x].city==v1){
      dex1=x;
    }
    if (vertices[x].city==v2){
      dex2=x;
    }
  }
  int d=vertices[dex1].adjacent.size();
  int e=vertices[dex2].adjacent.size();
  //iterates through adjacency list of each city and erases the other city fro it
  for (std::vector<vertex::path>::iterator it=vertices[dex1].adjacent.begin(); it!=vertices[dex1].adjacent.end(); ++it){
    if (vertices[it->index].city==v2){
      vertices[dex1].adjacent.erase(it);
      break;
    }
  }
  for (std::vector<vertex::path>::iterator ot=vertices[dex2].adjacent.begin(); ot!=vertices[dex2].adjacent.end(); ++ot){
    if (vertices[ot->index].city==v1){
      vertices[dex2].adjacent.erase(ot);
      break;
    }
  }
  std::cout<<"Path between "<<v1<<" and "<<v2<<" removed"<<std::endl;
}

void Graph::dijkstra(std::string v1, std::string v2){
  //dijkstra for distance
  int dex1;
  int dex2;
  int hold;
  int parent;
  int s=0;
  int prev[end];
  int pend=0;
  for (int d=0; d<end; d++){
    //making sure the sets get reset, or the algorithm will mess up if run multiple times
    vertices[d].distance=0;
    vertices[d].solved=false;
    vertices[d].parent=NULL;
  }
  //hold indexes of the two cities
  for (int z=0; z<end; z++){
    if (vertices[z].city==v1){
      dex1=z;
    }
    if (vertices[z].city==v2){
      dex2=z;
    }
  }
  //set origin node as solved
  vertices[dex1].solved=true;
  vertices[dex1].distance=0;
  vertices[dex1].parent=dex1;
  //creating a set of nodes already solved
  int solved[end];
  for (int j=0; j<end; j++){
    solved[j]=NULL;
  }
  solved[0]=dex1;
  //solved end keeps track of items in the array and the element to insert at
  int solvedend=1;
  while (!vertices[dex2].solved){
    //set min to infinity
    int minDistance=INT_MAX;
    int solvedV;
    for (int x=0; x<solvedend; x++){
      s=solved[x];
      for (int y=0; y<vertices[s].adjacent.size(); y++){
        if (!vertices[vertices[s].adjacent[y].index].solved){
          int dist=vertices[s].distance+vertices[s].adjacent[y].miles;
          //if some better distance is found, adjust
          if (dist<minDistance){
            solvedV=vertices[s].adjacent[y].index;
            minDistance=dist;
            parent=s;
            prev[pend]=s;
            pend++;
          }
        }
      }
    }
    //mark as solved, last solved is the end
    vertices[solvedV].distance=minDistance;
    vertices[solvedV].parent=parent;
    vertices[solvedV].solved=true;
    solved[solvedend]=solvedV;
    solvedend++;
  }
  int trav=dex1;
  std::cout<<"Shortest distance from "<<v1<<" to "<<v2<<" is "<<vertices[dex2].distance<<" miles:"<<std::endl;
  for (int i=1; i<pend; i++){
    std::cout<<vertices[prev[i]].city;
    std::cout<<" -> ";
  }
  std::cout<<v2<<std::endl;
  dex1;
  dex2;
  hold;
  parent;
  s=0;
  prev[end];
  pend=0;
  for (int d=0; d<end; d++){
    vertices[d].distance=0;
    vertices[d].solved=false;
    vertices[d].parent=NULL;
  }
  for (int z=0; z<end; z++){
    if (vertices[z].city==v1){
      dex1=z;
    }
    if (vertices[z].city==v2){
      dex2=z;
    }
  }
  vertices[dex1].solved=true;
  vertices[dex1].distance=0;
  vertices[dex1].parent=dex1;
  solved[end];
  for (int j=0; j<end; j++){
    solved[j]=NULL;
  }
  solved[0]=dex1;
  solvedend=1;
  while (!vertices[dex2].solved){
    int minDistance=INT_MAX;
    int solvedV;
    for (int x=0; x<solvedend; x++){
      s=solved[x];
      for (int y=0; y<vertices[s].adjacent.size(); y++){
        if (!vertices[vertices[s].adjacent[y].index].solved){
          int dist=vertices[s].distance+vertices[s].adjacent[y].time;
          if (dist<minDistance){
            solvedV=vertices[s].adjacent[y].index;
            minDistance=dist;
            parent=s;
            prev[pend]=s;
            pend++;
          }
        }
      }
    }
    vertices[solvedV].distance=minDistance;
    vertices[solvedV].parent=parent;
    vertices[solvedV].solved=true;
    solved[solvedend]=solvedV;
    solvedend++;
  }
  trav=dex1;
  std::cout<<"Shortest duration from "<<v1<<" to "<<v2<<" is "<<vertices[dex2].distance<<" minutes:"<<std::endl;
  for (int i=1; i<pend; i++){
    std::cout<<vertices[prev[i]].city;
    std::cout<<" -> ";
  }
  std::cout<<v2<<std::endl;
}

void Graph::dijkstra2(std::string v1, std::string v2){
  int dex1;
  int dex2;
  int hold;
  int parent;
  int s=0;
  int prev[end];
  int pend=0;
  for (int d=0; d<end; d++){
    vertices[d].distance=0;
    vertices[d].solved=false;
    vertices[d].parent=NULL;
  }
  for (int z=0; z<end; z++){
    if (vertices[z].city==v1){
      dex1=z;
    }
    if (vertices[z].city==v2){
      dex2=z;
    }
  }
  vertices[dex1].solved=true;
  vertices[dex1].distance=0;
  vertices[dex1].parent=dex1;
  int solved[end];
  for (int j=0; j<end; j++){
    solved[j]=NULL;
  }
  solved[0]=dex1;
  int solvedend=1;
  while (!vertices[dex2].solved){
    int minDistance=INT_MAX;
    int solvedV;
    for (int x=0; x<solvedend; x++){
      s=solved[x];
      for (int y=0; y<vertices[s].adjacent.size(); y++){
        if (!vertices[vertices[s].adjacent[y].index].solved){
          int dist=vertices[s].distance+vertices[s].adjacent[y].speed;
          if (dist<minDistance){
            solvedV=vertices[s].adjacent[y].index;
            minDistance=dist;
            parent=s;
            prev[pend]=s;
            pend++;
          }
        }
      }
    }
    vertices[solvedV].distance=minDistance;
    vertices[solvedV].parent=parent;
    vertices[solvedV].solved=true;
    solved[solvedend]=solvedV;
    solvedend++;
  }
  std::cout<<"Optimal route that balances speed and duration is: "<<std::endl;
  for (int i=1; i<pend; i++){
    std::cout<<vertices[prev[i]].city;
    std::cout<<" -> ";
  }
  std::cout<<v2<<std::endl;
}
