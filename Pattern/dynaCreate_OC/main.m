//
//  main.m
//  动态创建
//
//  Created by zhu on 2020/10/24.
//  Copyright © 2020年 zhu. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Person : NSObject
@property NSString* name;
-(instancetype)initWithName:(NSString*)name;
-(void)show;
@end
@implementation Person
-(instancetype)initWithName:(NSString*)name
{
    if(self=[super init]){
        _name = name;
    }
    return self;
}
-(void)show{
    NSLog(@"Person[%@]",_name);
}
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        Person* p = [Person new];
        Class clazz = [p class];
        Class clazz2 = [Person class];
        NSLog(@"Person class:%@",NSStringFromClass(clazz));
        
        NSString* str = @"Person";
        Class clazz3 = NSClassFromString(str);//根据名称获取Class对象
        NSLog(@"String class:%@",NSStringFromClass(clazz3));
        Person* pd = [[clazz3 alloc] initWithName:@"动态创建"];//类似于java 的class作用,基于C++动态创建机制
        [pd show];
        
        id obj = [[clazz2 alloc] initWithName:@"基于C++设计模式"];
        [obj show];
    }
    return 0;
}
