# Dynamic C 9
## for Rabbit-based embedded systems

### Description
This repository is the Open Source release of the libraries and sample
code from [Dynamic C 9][1], an integrated development environment for
[Digi International's](http://www.digi.com/) embedded systems based on
the Rabbit 2000 and 3000 microprocessors.

It is of limited use without the rest of the IDE and the appropriate
target hardware.  See the Installation section for instructions on installing
Dynamic C 9.62 and updating its libraries and samples with the code in
this repository.

The `release` branch is a **RELEASE** software release which is fully
QA-tested and supported by Digi International.
The `master` branch is an **ALPHA** software release which has received
limited functional/unit testing.

### Licensing
The [MPL 2.0 license](./LICENSE.txt) covers the majority of this project
with the following exceptions:

- The [ISC license](Samples/LICENSE.txt) covers the contents of the
  `Applications`, `RabbitLink` and `Samples` directories.

### Installation
Instructions on using Git and GitHub are beyond the scope of this document.
If you are new to using Git, we recommend the Windows GUI [Git Extensions][2].
Their website includes a manual and video tutorial.  The program has a
command-line "Git bash" tool, available in the Tools menu, that you can use
to execute the script below.

To make use of this code, either start with an existing (backed-up)
Dynamic C 9.62 installation, or download and install [Dynamic C 9.62][3],
followed by the [Embedded Security Pack][4] and finally, the 
[Dynamic C 9.62 Library Patches][5].  Portions of the Embedded Security 
Pack and Library Patches aren't tracked by this repository, so it is 
necessary to run those installers.

Next, add the GitHub repository to the directory.  These instructions
have you creating a private branch where you can store your own changes
to the libraries and samples, merging them in with Digi International's
changes.  It assumes you've already changed to the directory with Dynamic
C installed (e.g., `cd /c/DCRABBIT_9.62`).

    # Connect the Dynamic C installation with the GitHub repository,
    # and download all of the branches and tags.
    git init
    git remote add origin https://github.com/digidotcom/DCRabbit_9.git
    git fetch --tags
    
	# Configuration options to ignore file modes and preserve line endings
	# (since this is a Windows-only repository).
	git config core.filemode false
	git config core.autocrlf false
    
    # Reset to the patched 9.62 release (without changing directory contents).
	git reset 9.62-patch2
    
    # Get the .gitignore file from the repository.
    git checkout .gitignore
	
    # Create a private branch using the 9.62 release as a start point.
    git checkout -B mybranch

At this point, `git status` will show any changes to the directory
you have made since the original installation.  You should reset any
accidental changes, or create commits to track intentional changes.

Once you have processed all of your changes, you can cherry-pick
individual commits from the `master` and `release` branches, or merge
either branch, in its entirety, into yours.

[1]: http://www.digi.com/support/productdetail?pid=5053&type=software
[2]: http://gitextensions.github.io/
[3]: http://www.digi.com/support/downloads/index?pxIw8w5+g6A7mVbcVnuKyVipaKFPTmCz2GlkvLHWfRIJ0kDdobKybuYBFBtqriWC
[4]: http://www.digi.com/support/downloads/index?3PNGdN/1pOnGKl7dDsP8kplYtafJSamgzgYArj3a/+AS+OxNFqsZfq2RxCZpW488
[5]: http://www.digi.com/support/downloads/index?/BNu/a9U1RCNHvwfDYcNk/ph90s9aPFyt54h267Lv+Tsg6bTb7XlKDlZgzCUPlyh
