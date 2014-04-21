Viewing the Diagrams
====================

To view these diagrams in their picture form, you'll have to compile them with (PlantUML)[http://plantuml.sourceforge.net/].  If you're using Sublime Text 2 or above, you can use a (convinient plugin)[https://github.com/jvantuyl/sublime_diagram_plugin] to generate the images directly from your editor.

Mac
---

If you're using a Mac, you can install PlantUML and its dependency via Homebrew:

`brew install plantuml`
`brew install graphviz`

To install the plugin, and assuming you're using Sublime Text 3, you can just run `git clone git@github.com:jvantuyl/sublime_diagram_plugin.git ~/Library/Application\ Support/Sublime\ Text\ 3/Packages/sublime_diagram_plugin`.

Be sure to restart Sublime Text before you'll be able to use it.

Viewing with Sublime Text 3
---------------------------

- Open a .wsd file or create a new one
- Use Command-M to generate the image(s) relevant to the document

Creating New Diagrams
=====================

Please refer to the PlantUML (language documentation)[http://plantuml.sourceforge.net/download.html]

Compiling the Phase 1 and Phase 2 deliverables
==============================================

Use the makefile included in this folder.  Valid targets are all, phase-1, phase-2, and others described in the makefile.