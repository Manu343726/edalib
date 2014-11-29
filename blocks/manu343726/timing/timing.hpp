/* 
 * File:   scoped_clock.hpp
 * Author: manu343726
 *
 * Created on 23 de noviembre de 2014, 13:33
 */

#ifndef SCOPED_CLOCK_HPP
#define	SCOPED_CLOCK_HPP

#include <chrono>
#include <functional>
#include <tuple>
#include <utility>
#include <vector>
#include <algorithm>
#include <numeric>
#include <stdexcept>

#include <manu343726/portable_cpp/specifiers.hpp>

/**
 * This class manages global timing, maintaining a registry with the different timing frames
 * and the current timing stack.
 * 
 * You can registry timings using the functions start_frame() and finish_frame() directly, or instancing
 * a scped_clock local variable which does it automatically. 
 */
class timing_manager
{
public:
    using clock = std::chrono::high_resolution_clock;
    
    /**
     * This class represents a timing snapshot: The timing information of a function execution frame given by
     * a scoped_clock. It includes the time of timing start, the finish, and the total elapsed time of the frame.
     */
    struct snapshot
    {
    public:
        snapshot( const std::string& frame_function_name ) : 
            stack_frame( frame_function_name ) ,
            start_time( clock::now() ) , //Don't pass the time as parameter, compute here (Delay until the latest possible moment, to minimize bias)
            finished( false ) //Don't use uniform initialization to prevent MSVC C2797 error
        {}
            
        void finalize( const clock::time_point& finish ) //Since this is the finish, its the inverse of the ctor: Pass the time as param, to reduce bias (Compute the time in the first possible moment)
        {
            if(finished) throw std::logic_error{ "This frame is already finished!" };
            
            finish_time = finish;
            finished = true;
        }
        
        clock::time_point start() const NOEXCEPT
        {
            return start_time;
        }
        
        clock::time_point finish() const
        {
            if(!finished) throw std::logic_error{ "Timing frame not finished!" };
            
            return finish_time;
        }
        
        auto elapsed() const -> decltype(finish() - start())
        {
            return finish() - start();
        }
        
        std::string frame_function() const
        {
            return stack_frame;
        }
        
        friend bool operator==(const snapshot& a , const snapshot& b)
        {
            //This relaxed comparison allows comparing a finished snapshot with a non-finished one. Why?
            //Suppose you take and store the snapshot during on_start() to use it in the future. Execpt you store the reference,
            //your snapshot will not be updated (And storing the reference you could invoke UB, since the registry storage can be invalidated).
            //Two timing frames later, you compare your snapshot with the latest one registered. Even if the snapshot registered you toke has been finalized,
            //your's is not (Since its a copy). Then you want to be able to search for your snapshot on the registry.
            return std::tie(a.start_time,a.stack_frame) 
                   ==
                   std::tie(b.start_time,b.stack_frame);
        }
        
        friend bool operator!=(const snapshot& a , const snapshot& b)
        {
            return !(a==b);
        }
        
    private:
        clock::time_point start_time;
        clock::time_point finish_time;
        std::string stack_frame;
        bool finished;
    };
    
    /**
     * Registers a new timing frame and starts its timing.
     * @param frame_function_name Name of the function to be timed.
     */
    static void start_frame(const std::string& frame_function_name )
    {
        timing_stack.emplace_back( frame_function_name );
        
        if(_on_start) 
            _on_start(timing_stack.back());
    }
    
    /**
     * Finalizes the latest timing frame, filling its timing data.
     */
    static void finish_frame()
    {
        auto now = clock::now(); //Compute time here to minimize timing bias
        
        if(timing_stack.empty()) throw std::logic_error{ "The call stack is empty!" };
        
        timing_registry.push_back( std::move(timing_stack.back()) ); //Pass the snapshot from the stack to the registry
        timing_stack.pop_back();
        timing_registry.back().finalize(now); //Finalize the snapshot
        
        if(_on_finish)
            _on_finish(timing_registry.back());
    }
    
    /**
     * Sets the action to be executed just after a timing frame starts.
     * Can be used to print some logging info, registering timing events, etc.
     * @param f The action to be executed. Has signature void(const snapshot&).
     */
    template<typename F>
    static void on_start(F f)
    {
        _on_start = f;
    }
    
    /**
     * Sets the action to be executed just after a timing frame starts.
     * Can be used to print some logging info, registering timing events, etc.
     * @param f The action to be executed. Has signature void(const snapshot&).
     */
    template<typename F>
    static void on_finish(F f)
    {
        _on_finish = f;
    }
    
    /**
     * Returns the current timing registry
     * @return 
     */
    static const std::vector<snapshot>& registry()
    {
        return timing_registry;
    }
    
    /**
     * Returns the latest registered snapshot
     */
    static snapshot last()
    {
        if(timing_registry.empty()) throw std::logic_error{ "The registry is empty!" };
        
        return timing_registry.back();
    }
    
    /**
     * Returns the total elapsed time between two snapshots
     * @param begin first snapshot
     * @param end last snapshot
     * @return a clock::time_point with the total elapsed time
     */
    static clock::time_point elapsed( const snapshot& begin , const snapshot& end )
    {
        auto bit = std::find(std::begin(timing_registry) , std::end(timing_registry) , begin );
        auto eit = std::find(std::begin(timing_registry) , std::end(timing_registry) , end );
        
        if( bit == std::end(timing_registry) || eit == std::end(timing_registry) ) throw std::logic_error{ "The snapshots should be registered!" };
        
        //It the user passed the snapshots in the wrong way (begin is older than end), swap them....
        if( eit < bit )
            std::iter_swap( eit , bit );
        
        //Note for Haskellers: std::accumulate() is foldl
        //eit++ since C++ ranges are [begin,end)
        return std::accumulate( bit , eit++ , clock::time_point::min() , [](const clock::time_point& acc , const snapshot& current )
        {
           return acc + current.elapsed(); 
        });
    }
    
    /**
     * Returns the total registered elapsed time
     * @return 
     */
    static clock::time_point elapsed()
    {
        if(timing_registry.empty()) throw std::logic_error{ "The registry is empty!" };
        
        return elapsed(timing_registry.front() , timing_registry.back());
    }
    
private:
    //Be careful, this implementation supposes a single-threaded app.
    static std::vector<snapshot> timing_stack; //Set of snapshots of the current timing call stack
    static std::vector<snapshot> timing_registry; //Stores a registry with all the timings (snapshots) performed
    static std::function<void(const snapshot&)> _on_start; //Action to be performed just after frame start (Printing log info, for example)
    static std::function<void(const snapshot&)> _on_finish; //Action to be performed just after frame finish (Printing log info, for example)
};

class scoped_clock
{
public:
    
    //A scoped clock is not default constructible, copy constructible, move constructible,
    //copy assignable, nor move assignable.
    scoped_clock() = delete;
    scoped_clock(const scoped_clock&) = delete;
    scoped_clock(scoped_clock&&) = delete;
    scoped_clock& operator=(const scoped_clock&) = delete;
    scoped_clock& operator=(scoped_clock&&) = delete;
    
    
    scoped_clock(const std::string& function_name)
    {
        timing_manager::start_frame( function_name );
    }
    
    ~scoped_clock()
    {
        timing_manager::finish_frame();
    }
};

#if defined(ENABLE_TIMING) && !defined(NDEBUG)
#define SCOPED_CLOCK scoped_clock my_awesome_name_i_hope_you_will_never_use_for_local_variables{__FUNCTION__};
#else
#define SCOPED_CLOCK
#endif /* TIMING_ENABLED AND DEBUG MODE */
#endif	/* SCOPED_CLOCK_HPP */

