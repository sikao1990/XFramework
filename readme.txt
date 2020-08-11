Window/Linux
	锁//lock
	信号量//semaphore
	高级信号量//advanced semaphore
	线程//thread
	线程池//thread pool ,task pool
	多线程同步//thread sync,task sync
	生产者消费者同步//Producer consumer
	定时器//timer
	读写锁//rwlock

Description
    Partly not completed, and partly not tested
    
Bug
    Current importance Bug
        window vs2015 x64 debug: Run-Time Check Failure #2 - Stack around the variable '_result' was corrupted.
            x64 release.x86 debug/release all ok
            
        test.cpp :pDBAccess->Query("select pic from example_blob where id=99",reslst);
        exception/break point: ado long lDataSize = pRecordset->GetFields()->GetItem("pic")->ActualSize;
    
