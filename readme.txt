Window/Linux
	��//lock
	�ź���//semaphore
	�߼��ź���//advanced semaphore
	�߳�//thread
	�̳߳�//thread pool ,task pool
	���߳�ͬ��//thread sync,task sync
	������������ͬ��//Producer consumer
	��ʱ��//timer
	��д��//rwlock

Description
    Partly not completed, and partly not tested
    
Bug
    Current importance Bug
        window vs2015 x64 debug: Run-Time Check Failure #2 - Stack around the variable '_result' was corrupted.
            x64 release.x86 debug/release all ok
            
        test.cpp :pDBAccess->Query("select pic from example_blob where id=99",reslst);
        exception/break point: ado long lDataSize = pRecordset->GetFields()->GetItem("pic")->ActualSize;
    
