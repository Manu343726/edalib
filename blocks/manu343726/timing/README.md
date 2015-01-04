manu343726/timing
=================

A simple tool for C++ function timing

Description
-----------

Since C++11 the C++ Standard Library includes the `<chrono>` library, very convenient to perform perfomance diagnostics and timing in general.
But to do timing to a function execution you still have to manage all the setup, getting the time points and substracting them to get the elapsed time:


    int main()
    {
        auto begin = std::chrono::high_resolution_clock::now();
    
        f(); //Call to be measured
    
        auto end = std::chrono::high_resolution_clock::now();
    
        std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms";
    }

What this library provides is a way to manage timing of a set of relevant function calls, storing all the individual timings as a registry of snapshots.

    void f()
    {
        SCOPED_CLOCK; //The magic occurs here
    
        std::this_thread::sleep_for( std::chrono::milliseconds{10} );
    }
    
    void log( const timing_manager::snapshot& snapshot )
    {
        std::cout << "Call to " << snapshot.frame_function() << " finished ("
                  << std::chrono::duration_cast<std::chrono::milliseconds>(snapshot.elapsed() ) 
                  << " ms)" << std::endl;
    }
    
    int main()
    {
        timing_manager::on_finish(log);
    
        f();
        f();
        f();
    
        std::cout << "Total elapsed time: " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(timing_manager::elapsed()) 
                  << " ms" << std::endl;  
    }

> Call to f finished (10 ms)  
> Call to f finished (10 ms)  
> Call to f finished (10 ms)  
> Total elapsed time: 30 ms  

The timing can be disabled/enabled easily using `bii cpp:configure`, just set the cmake variable `TIMING` to `ON` or `OFF`:

    $ bii cpp:configure -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DTIMING=ON

*Note the timing is enabled only in Debug compilation.*