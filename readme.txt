Cross-platform application framework library by C++ Independent of UI

Support Window/Linux/MacOS
    Programmers no longer need to pay attention to the interface at the operating system level, 
    but can focus on logic issues like an interpreted scripting language such as Java

Directory Structure
    DB       :Interface encapsulation is independent of the specific database, 
              similar to the operation of Java on the database

    DyPlugin :dll/so/dylib 
    Mem
    Net      :tcp/udp 
              No need to pay attention to soket anymore, only need to encode or decode data protocol
    SharedMemory
    Thread   :Principles of java thread implementation
    SystemApi
    misc
    Pattern  :Implementation of C++'s most fundamental design pattern
                like MFC,Objective-C,Java's dynamic creation mode,
    example  :test code
    
Window/Linux/MacOS
	lock
	semaphore
	advanced semaphore
	thread
	thread pool ,task pool
	thread sync,task sync
	Producer consumer
	timer
	rwlock
    IOCP/Epoll/Kqueue

Description
    Partly not completed, and partly not tested
    
Bug
    Current importance Bug
    1:DB
        window vs2015 x64 debug: Run-Time Check Failure #2 - Stack around the variable '_result' was corrupted.
            x64 release.x86 debug/release all ok
            
        test.cpp :pDBAccess->Query("select pic from example_blob where id=99",reslst);
        exception/break point: ado long lDataSize = pRecordset->GetFields()->GetItem("pic")->ActualSize;
    2:SystemApi - Service_Win32
    	bug:MYSERVICE stop first,when TESTSERVICE stop,the MYSERVICE was restart,others was normal

author:Zhuliwu
email:sikao1990@outlook.com/skyrisk1990@gmail.com

