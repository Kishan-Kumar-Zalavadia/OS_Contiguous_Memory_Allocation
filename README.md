This project contains three program files
1. MemoryAllocator.cpp
	This runs the memory allocation algorithm for a single input txt file.
	
	Compile the code:
	    g++ -o MemoryAllocator MemoryAllocator.cpp
	 Run the code:
  		./MemoryAllocator file_name	

2. MemoryAllocatorTestCaseGenerator.java
	This Java program generates test cases for the program. It creates an "inputs" folder in which all the test cases are stored.
	
	Compile the code:
	    javac MemoryAllocatorTestCaseGenerator.java
	Run the code
	    java MemoryAllocatorTestCaseGenerator
	
3. MultiFileMemoryAllocator.cpp
	This program runs all the test cases and displays the results of all the test cases at once.
	Compile the code:
    		g++ -std=c++17 -o MultiFileMemoryAllocator MultiFileMemoryAllocator.cpp
	Run the code:
		./MultiFileMemoryAllocator

Steps to run the project:
1. To run a single file, compile and run MemoryAllocator.cpp as shown in above point 1.
2. To generate and run the test cases, compile and run above points 2 and 3 inline.

Source code: https://github.com/Kishan-Kumar-Zalavadia/OS_Contiguous_Memory_Allocation
