#include "project.h"
#include "project.cpp"

int main(){
  Graph map;
  int menu=0;
  while (menu==0){
      std::cout<<"======Main Menu======"<<std::endl;
      std::cout<<"1. Display the current cities and paths in the map"<<std::endl;
      std::cout<<"2. Find if a city is in the map"<<std::endl;
      std::cout<<"3. Add a city to the map"<<std::endl;
      std::cout<<"4. Add a route between cities in the map"<<std::endl;
      std::cout<<"5. Delete a city from the map"<<std::endl;
      std::cout<<"6. Delete a path from the map"<<std::endl;
      std::cout<<"7. Find the optimal route between two cities in the map"<<std::endl;
      std::cout<<"8. Quit"<<std::endl;
      std::string choice;
      getline(std::cin, choice);
      if (choice=="1"){
        map.printGraph();
      }
      if (choice=="2"){
        std::string search;
        std::cout<<"Please enter the city you wish to search for:"<<std::endl;
        getline(std::cin, search);
        map.search(search);
      }
      if (choice=="3"){
        std::string add;
        std::cout<<"Please enter the city you wish to add to the map:"<<std::endl;
        getline(std::cin, add);
        map.insertPlace(add);
      }
      if (choice=="4"){
        std::string one;
        std::string two;
        std::cout<<"Please enter the first city:"<<std::endl;
        getline(std::cin, one);
        std::cout<<"Please enter the second city:"<<std::endl;
        getline(std::cin, two);
        if (map.foundinmap(one, two)==1){
        }
        else if (map.foundinmap(one, two)==2){
          std::cout<<"Path already exists"<<std::endl;
        }
        else if (map.foundinmap(one, two)==3){
          map.findPath(one, two);
        }
      }
      if (choice=="5"){
        std::string del;
        std::cout<<"Please enter the city to be deleted:"<<std::endl;
        getline(std::cin, del);
        map.deletePlace(del);
      }
      if (choice=="6"){
        std::string three;
        std::string four;
        std::cout<<"Please enter the first city:"<<std::endl;
        getline(std::cin, three);
        std::cout<<"Please enter the second city:"<<std::endl;
        getline(std::cin, four);
        if (map.foundinmap(three, four)==1){
        }
        else if (map.foundinmap(three, four)==3){
          std::cout<<"Path does not exist"<<std::endl;
        }
        else if (map.foundinmap(three, four)==2){
          map.deletePath(three, four);
        }
      }
      if (choice=="7"){
        std::string five;
        std::string six;
        std::cout<<"Please enter the starting city:"<<std::endl;
        getline(std::cin, five);
        std::cout<<"Please enter the destination city:"<<std::endl;
        getline(std::cin, six);
        if (map.foundinmap(five, six)==1){
        }
        else {
          map.dijkstra(five, six);
          map.dijkstra2(five, six);
        }
      }
      if (choice=="8"){
        std::cout<<"Goodbye!"<<std::endl;
        menu=1;
      }
  }
  //map.findPath("Seattle WA", "Portland OR");
  return 0;
};
