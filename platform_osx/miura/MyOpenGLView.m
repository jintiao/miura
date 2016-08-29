//
//  AppDelegate.m
//  miura
//
//  Created by jintiao on 16/8/29.
//  Copyright © 2016年 jintiao. All rights reserved.
//

#import "MyOpenGLView.h"
#include <OpenGL/gl.h>
 
@implementation MyOpenGLView
-(void) drawRect: (NSRect) bounds
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}
@end