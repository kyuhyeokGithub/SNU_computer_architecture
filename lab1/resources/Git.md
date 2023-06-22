# Git Revision Control System {#Git}

This is an absolutely bare-minimum Git how-to. For more a more detailed tutorial, visit the [Git How To](https://githowto.com/), one of the many other tutorials available on the net, or ask ChatGPT.

[TOC]

# Cloing a directory
Copy the repository's https URL from the webinterface and, in a terminal, run
~~~
$ git clone <URL>
~~~


# Downloading ("pulling") the latest version from the remote server
To update your local repository with the newest version from the remote, run
~~~
$ git pull
~~~


# Creating a local repository
In directory, run
~~~
$ git init
~~~
You have now a it repository that covers the current directory and all its subdirectories.


# Editing
After modifying the contents of your repository, enter
~~~
$ git add <file>
$ git rm <file>
~~~
to add or remove files from the next change.
~~~
$ git status
~~~
shows you the status of new, modified, or deleted files.


# Reverting the changes made to a file
If you have modified a file, but want to restore it to the version in the repository, run
~~~
$ git restore <file>
~~~
Note that this only works _before_ a commit.


# Committing
To commit your changes to the local repository, run
~~~
$ git commit -m "<Commit message>"
~~~


# Uploading ("pushing") your local changes to the remote server
To synchronize the remote server with your local repository, run
~~~
$ git push
~~~


# Inspecting all commits {#gitlog}
List all commits with
~~~
$ git log
~~~
Another very useful command is 
~~~
$ git whatchanged
~~~
that shows you the commits along with the modified files.


# Going back to a specific commit
If you make your solution worse instead of better, you can revert your changes and go
back to a commit. List the commits as shown above, select the one you want to go back to,
and execute
~~~
$ git reset --hard <commit id>
~~~
where &lt;commit id&gt; is the commit hash of the commit you want to revert to (see [Inspecting all commits](@ref gitlog) above).

*Careful*: this will erase any changes made that never been commited.

