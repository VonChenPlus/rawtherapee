# About
RawTherapee is a cross-platform raw image processing program, released under the GNU General Public License Version 3. It is written in C++, using a GTK+ front-end and a patched version of dcraw for reading raw files. It is notable for the advanced control it gives the user over the demosaicing and developing process.

Please check the official RawTherapee homepage for further details, features, screenshots, downloads, etc.

# Source Code Tarballs
Download source code tarballs from: http://rawtherapee.com/shared/source/

Note: Google Code stopped hosting downloads and the tarball links at code.google.com/p/rawtherapee/downloads/ are likely to stop working at any time, therefore please use the rawtherapee.com/shared/source/ link.

# Contributing
All people willing to contribute to RawTherapee by beta-testing, coding or making translations are welcome! Ideally, we need beta-tester and developers for all supported platforms (OS X/Linux/Windows).

This project uses CMake to create the cross platform Makefile, but the only supported toolchain is GNU/GCC (4.7 or higher) so far, mainly through the Eclipse IDE, complemented by Mercurial as a team-working tool. As requested by some developers, we're looking for people that would integrate other toolchains like VisualStudio and its compiler to the supported toolchain.

The development contribution process is as follows:

open an issue to explain the patch you're willing to code (bug description, new feature description, etc...)
code your patch by taking into account other devs/users comments
post the patch in your issue
wait for another dev to test your patch
if no one objects after at least 1 day (for the smallest patches), you can ask someone to commit it to the "default" branch.
If you're willing to code on a regular basis, and if actual devs are okay after reviewing your patches, we can grant you commit rights if you require them.

__Important__

While small patches do not take much time to code, bigger features or code refactoring may take much longer and affect more source files. In the past, we used to create separate branches to code them, with regular intermediate merges from "default" to the new branch to keep them in synch. This is still possible, but regardless of whether you do that in a separate branch or through a posted patch, you/we still have to undertake the task of decomposing large changes into smaller & committable patches. This is necessary to avoid being out of synch at some point. Then you'll be able to commit/merge those intermediary patches from the new branch into "default", but still leave RawTherapee in a buildable and usable state.

# Source Code Documentation
That's certainly one of the problems you'll encounter while developing for RawTherapee, the in-source documentation is not as complete as we would have wanted. But you'll still be able to generate it through the use of Doxygen. If you don't have the required setup to do it yourself, you can find it on Michael Ezra's website (many thanks!). Beware, it may not be up-to-date.

# Roadmap & Future Plans
The roadmaps are handled through GoogleCode's issue list. Just look for them by typing "label:Type-Roadmap" in the search bar, or clicking on this link: Roadmaps

Here is a rough list of mid and long-term plans or features, we'd need your help in making them happen:

be able to possibly use different GUI toolkits, which means that the GUI should be based on configuration files (XML) and tools should be modularized. This implies a major code refactoring. See Jan Rinze's work about that with his rtviewer project.
supporting uncommon sensor types
use hardware acceleration as much as possible (constant enhancement)
switch from PP3 to XMP
add local editing
add pipette tools for most curves
add watermarking
add node editing (layers)... can only be done after the code rewrite
add a plugins API
add soft-proofing
maybe able to run on tablets ("would be nice" ;) )... can only be done after the code rewrite
maybe enhance and separate the DAM features from the (batch) development feature
maybe integrate peripheral features to be a full LR-like software (i.e. Printing, Uploading, etc..), but with a really low priority

__Any help from you will be much appreciated.__
