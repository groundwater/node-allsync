#include <stdio.h>
#include <iostream>

#include <v8.h>
#include <node.h>

using namespace v8;
using namespace node;

int MAX_BUFFER = 100;

static Handle<Value> NodeExec(const Arguments& args) {
  
  HandleScope scope;
  
  int length = args.Length();
  
  // Expect a callback function for stdout
  Handle<String> Command = Handle<String  >::Cast(args[0]);
  Handle<Function> cbout = Handle<Function>::Cast(args[1]);
  
  // Get Command
  char* command( *v8::String::Utf8Value(Command) );
  
  char buffer[MAX_BUFFER];
  
  FILE *stream = popen( command, "r");
  
  Handle<Value> argv[1];
  while ( std::fgets(buffer, MAX_BUFFER, stream) != NULL )
  {
    // Do callback here!
    argv[0] = String::New( buffer );
    if(length>1)
      cbout->Call(Context::GetCurrent()->Global(), 1, argv);
  }
  
  int code = pclose(stream);
  
  return scope.Close(Integer::New(code));
}

static void init(Handle<Object> target) {
  target->Set(String::NewSymbol("exec"),
    FunctionTemplate::New(NodeExec)->GetFunction());	
}

NODE_MODULE(allsync, init)
