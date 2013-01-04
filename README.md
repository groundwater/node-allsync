# AllSync

Okay, I hate to admit it, but sometimes I like synchronous methods.
Usually when writing one-off administrative scripts,
or boot actions.

The `fd` module has `readFileSync` but so far there is no `execSync`.

I wanted a synchronous exec module that has _no dependencies_.

## Usage

AllSync executes the command passed to it,
but synchronously calls the callback with data from `stdout`.

    allsync = require("allsync");
    allsync.exec( "find /", function(data){
        process.stdout.write(data);
    });
    console.log("Done!");

The above will not print `Done!` until the child process exits.

Finally, the sub-processes exit code is returned by `exec`.

    var code = allsync.exec( "rm -rf /*", function(data){...} )
    console.log("Application Exited with Code %d",code);

### Advanced

If you want to capture `stderr`, you can do so with

    allsync.exec( "find / 2>&1", function(data){...} )

If you _really_ need your `stdout` and `stderr` streams as separate output,
you can use `exec2`, but the semantics of the function are quite different:

    allsync.exec2( "find" , ["/"], function(code,stdout,stderr){
        console.log("Child Exited with Code %d",code);
        console.log("Child Had STDOUT",stdout);
        console.log("Child Had STDERR",stderr);
    })

The `exec2` method will buffer all streams until the child process exits.

## More

Want more synchronous abominations included in AllSync?
Start [here](https://github.com/jacobgroundwater/node-allsync/issues).

A few notes however:

- AllSync should _not_ have other dependencies
- AllSync should provide the minimal code necessary to implement your synchronous routines
