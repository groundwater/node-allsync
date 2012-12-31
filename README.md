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

### Advanced

At this time, AllSync does not capture `stderr`,
if you want it you will have to redirect stderr in your command.

    allsync.exec( "find / 2>&1", function(data){...});

Finally, AllSync returns with the exit code of the sub process.

    var code = allsync.exec( "rm -rf /", function(data){...});
    console.log("Child Process Exited with Code %d", code);
