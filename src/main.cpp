#include <stdio.h>
#include <iostream>

#include <v8.h>
#include <node.h>

using namespace v8;
using namespace node;

int MAX_BUFFER = 100;

// Copy string content to a new heap object
char* getAscii( Handle<String> item ){
  char* temp( *String::AsciiValue(item) );
  char* out = new char [sizeof(temp)];
  strcpy(out,temp);
  return out;
}

static Handle<Value> NodeExec2(const Arguments& args) {
  HandleScope scope;
  
  // Expect a callback function for stdout
  Handle<String> Command = Handle<String  >::Cast(args[0]);
  Handle<Array> ArgArray = Handle<Array   >::Cast(args[1]);
  Handle<Function>    Cb = Handle<Function>::Cast(args[2]);
  
  // Create Pipes for STDIN, STDOUT, and STDERR //
  
  int PIPE_READ  = 0;
  int PIPE_WRITE = 1;
  
  int pipe_stdin [2]; // [read,write]
  int pipe_stdout[2];
  int pipe_stderr[2];
  
  pipe (pipe_stdin);
  pipe (pipe_stdout);
  pipe (pipe_stderr);
  
  pid_t pid;
  pid = fork();
  
  if(pid == (pid_t) 0)
  {
    // Child //
    
    char* command = getAscii(Command);  
    
    int arrayLen = ArgArray->Length();
    char* pargs[arrayLen + 2];
    
    // Create Argument Array from JavaScript Array
    pargs[0] = command;
    for(int i=0; i < arrayLen; i++)
    {
      pargs[i+1] = getAscii(Handle<String>::Cast( ArgArray->Get(i) ));
    }
    pargs[arrayLen+1] = '\0';
    
    // Close Unused Pipe Ends for Child
    close( pipe_stdin [PIPE_WRITE] );
    close( pipe_stdout[PIPE_READ]  );
    close( pipe_stderr[PIPE_READ]  );
    
    // Switch Standard File Descriptors to IPC Pipes
    dup2( pipe_stdin [PIPE_READ ], STDIN_FILENO  );
    dup2( pipe_stdout[PIPE_WRITE], STDOUT_FILENO );
    dup2( pipe_stderr[PIPE_WRITE], STDERR_FILENO );
    
    
    int code = execvp(command, pargs);
    
    _exit(code);
    
  }
  else
  {
    // Parent
    
    std::printf("Receiving Output from Child %d\n",pid);
    
    // Close Unused Ends for Parent
    close( pipe_stdin [PIPE_READ]  );
    close( pipe_stdout[PIPE_WRITE] );
    close( pipe_stderr[PIPE_WRITE] );
    
    std::string stdout;
    std::string stderr;
    
    char buffer;
    
    while( read(pipe_stdout[PIPE_READ], &buffer, 1 ) > 0 )
    {
      stdout.append( 1, buffer );
    }
    
    while( read(pipe_stderr[PIPE_READ], &buffer, 1 ) > 0 )
    {
      stderr.append( 1, buffer );
    }
    
    int status;
    waitpid(pid, &status, 0);
    
    Handle<Value> argv[3];
    argv[0] = Integer::New(status);
    argv[1] = String::New(stdout.c_str());
    argv[2] = String::New(stderr.c_str());
    
    Cb->Call( Context::GetCurrent()->Global(), 3, argv );
    
  }
  
  return scope.Close(Undefined());
}

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
  int status;
  
  waitpid(-1, &status, 0);
  
  return scope.Close(Integer::New(code));
}

static void init(Handle<Object> target) {
  target->Set(String::NewSymbol("exec"),
    FunctionTemplate::New(NodeExec)->GetFunction());	
  target->Set(String::NewSymbol("exec2"),
    FunctionTemplate::New(NodeExec2)->GetFunction());	
}

NODE_MODULE(allsync, init)
