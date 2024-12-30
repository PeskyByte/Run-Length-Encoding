Performance Measurement 

Using huge input: for the first test, we use an input file with more than a million lines inside it, we count the lines using wc command in Linux. We also measure the time twice, once inside the code and once using the time command in Linux. This command measures the the total time from calling the run command until finishing it. 
 
![Screenshot from 2024-12-30 21-50-09](https://github.com/user-attachments/assets/109d3247-a074-4752-8281-75084db0d61c)


We notice that for a very large input, the MPI implementation has the best performance out of the three implementations, although it takes some overhead kernel time to create the processes, it is the fastest to process the data. 

The threaded approach also provides a huge improvement over the sequential approach, we can also notice the very little overhead for creating the threads. 




The second test is done using a smaller size input with 500,000 lines 
![Screenshot from 2024-12-30 22-00-23](https://github.com/user-attachments/assets/a02ef6e0-ffdc-49a8-9bf1-4ffaa7934f80)

We notice that the MPI approach here takes a second of kernel time to create the processes and then performs better than the threaded with regards to computation. But the overall time for the threaded implementation for this input size is less than the overall time of the MPI implementation. 

We also notice that the difference between the sequential approach and the threaded approach is getting smaller. 




For the last test, we use a smaller input size of 50,000 lines 

 ![Screenshot from 2024-12-30 22-06-18](https://github.com/user-attachments/assets/5e23bdec-c1e0-454f-98e7-2a80502638c0)

We notice that with small inputs the overhead for the MPI implementation makes it not efficient as the threaded implementation. Even the sequential implementation performed better than the MPi implementation for this test case. 

The threaded implementation performed the best of the three, but the difference between the threaded and the sequential is way smaller than before. This indicates that for a smaller input, the sequential implementation might perform better than the threaded one. 


 
In conclusion, for computation extensive workloads, a multi processed implementation will perform the best. For a medium workload, the threaded approach is the best option. This is due to the expensive overhead that the MPI approach has when creating processes. 
