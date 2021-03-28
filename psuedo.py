quanta = 0
runQueues[31]
waitQueue
zombieQueue

def ProcessSchedule():
    cur_jiffy = ClkGetCurJiffies()

    disableInterrupts()

    quanta += 1

    # Deals with process on the wait queue and those sleeping with a timer
    empty = runQueuesEmpty()
    if empty:
        if waitQueue not empty:
            timed_sleeping = willProcsBeWoken()
            if not timed_sleeping:
                exit
        if not timed_sleeping:
            exit
    # Check for processes to wake up from slumber
    for pcb in waitQueue:
        if on_timer and time_to_wake_up:
            Wakeup(pcb)

    pcb.window_jiffies = cur_jiffy - pcb.start_jiffies
    pcb.cumul_jiffies += pcb.window_jiffies
    if(pcb.pinfo) print(pcb.cumul_jiffies)

    RecalcPriority(pcb)
    ''' If proc is yielded, it doesn't increment.
    This function checks window jiffies to see how much cpu
    time was used '''
    FixRunQueues(pcb)
    
    if quanta >= 10:
        DecayAllEstcpus()
        FixRunQueues() 
        quanta = 0
    
    pcb = FindHighestPriorityPCB()
    if pcb is idleProcess or pcb is the_same:
        num = WhichQueue(pcb)
        AQueueMoveAfter(&(runQueues[num]), AQueueLast(&(runQueues[num])), AQueueFirst(&(runQueues[num])))
        pcb = ProcessFindHighestPriorityPCB()

    pcb.start_jiffie = cur_jiffy

    FreeZombies()

    RestoreInterrupts()

    return