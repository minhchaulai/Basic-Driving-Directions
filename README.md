# 2270 Project: Driving Routing Optimization - Minhchau Lai

## Routing optimization 
This program creates a map of cities and routes to other cities through a weighted graph via adjacency list. Users are able to add and delete cities and routes between cities to the map, and the Google Maps API will automatically request the data for the route, enabling users to add basically any place to the map as long as driving directions are available. Using Dijkstra's algorithm, users can calculate the route between two selected cities through the cities currently in the map. The optimal routes for time spent and distance driven are then displayed, and the optimal route balancing distance and time is given.

## How to Run and Dependencies
This program is for Mac and Linux systems. 
The program is heavily reliant on Microsoft's C++ Rest SDK (Casablanca), available at https://github.com/Microsoft/cpprestsdk. The repository should contain installation instructions in the documentaiton to install cpprestsdk. cpprestsdk also has its own dependencies, including Boost and OpenSSL. In particular, cpprestsdk should be built with OpenSSL to be able to handle the https requests to Google. Once cpprestsdk library is installed, download the header, class, and driver files in the master of this repository. Compile the program with G++, and C++11 in order for the program to work. For many systems, the Boost library's include files are not searched by G++, so arguments at compile are necessary. 

Here are the recommend compiler settings:
**g++ -std=c++11 pmain.cpp -o pmain -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include -stdlib=libc++ -lcpprest -lssl -lcrypto -lboost_system -lboost_thread-mt -lboost_chrono-mt -g**

## Current bugs and enhancements
Currently, there is a bug where routes may display the same step in succession. While not breaking the functionality, this does make it look unclean. This is likely due to the data being stored as the index location of the node in the array, rather than a pointer to the node itself. Thus, this is the most important enhancement to make in future versions. 
