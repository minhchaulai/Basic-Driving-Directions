#ifndef PROJECT_HPP
#define PROJECT_HPP

#include <iostream>
#include <vector>
#include <atomic>
#include <sstream>
//algorithm for vector erase-remove operations
#include <algorithm>
#include <string>
//file includes from the SDK library
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/json.h>

//nest path inside vertex so we can declare a vector of paths

struct vertex{
  std::string city;
  struct path{
    //we'll be storing indexes to nodes instead of pointers, I had issues getting pointers to work properly
      int index;
      float miles;
      float time;
      float speed;
      path(){}
      path(int i, float m, float t){
        index=i;
        miles=m;
        time=t;
        speed=round(miles/time);
      }
  };
  std::vector<path> adjacent;
  //variables required for dijkstra's algorithm
  bool solved;
  int distance;
  int parent;
  vertex(){}
  vertex(std::string name){
    city=name;
    solved=false;
    distance=0;
    parent=NULL;
  }
};

class Graph{
  public:
    Graph();
    ~Graph();
    void insertPlace(std::string city);
    void insertPath(std::string v1, std::string v2, float m, float t);
    void findPath(std::string v1, std::string v2);
    void deletePlace(std::string v1);
    void deletePath(std::string v1, std::string v2);
    void printGraph();
    void search(std::string v1);
    void findShortest();
    int foundinmap(std::string v1, std::string v2);
    void dijkstra(std::string v1, std::string v2);
    void dijkstra2(std::string v1, std::string v2);
    void dijkstra3(std::string v1, std::string v2);
  private:
    void astar();
    int end;
    //declaring an array of 100 elements. as a cap.
    vertex vertices[100];
  };
#endif // PROJECT_HPP
