###########################
Introducing ``citeproc-js``
###########################

.. include:: <isonum.txt>
.. include:: <isopub.txt>

Citation copy-editing is one of those deceptively small burdens that
have a way of taking over the working day.  If left untended, the task
of tidying up casually scribbled references can snowball to crisis
proportions as a submission deadline approaches.  Similarly, when a
submission to one publisher is unsuccessful, significant effort may be
required to recast its citations in the format required by another.  
Collaboration outside of one's own field can bring with it an
unwelcome tangle of fresh style-guide quandaries to ponder and fight
through.  These are things that the machines, if they want to make
themselves useful, should be doing for us.

There is plenty of collective experience in this line, and as fate
would have it, there are also plenty of collective solutions.  In the
TeX/LaTeX world, authors and their editors can today choose between
BibTeX and BibLaTeX |mdash| both of them excellent utilities |mdash|
with the several variants of the former supported by no fewer than
four separate versions of the BibTeX program. [#]_ Users of WYSIWYG
word processors can look to the bibliographic support built into Word
or Open Office, or they can turn to an external solution such as
EndNote |trade|, ProCite |trade|, Reference Manager |trade|, or more
recently Zotero or Mendeley.  Migrating data between these
environments is a process fraught with uncertainty, but it is
sometimes unavoidable when you need this kind of output, and it can
only be produced on that kind of system |hellip|

|hellip| with so many solutions to choose from, it's hard to go right. [#]_

The ``citeproc-js`` citation processor is a Javascript implementation
of the *Citation Style Language* (CSL), an XML schema for describing
citation styles that aspires to strike this problem at its root.  CSL
is a general, open standard that enables fully modular control over
bibliographic formatting.  This means that CSL is capable of
accurately describing styles used in many disciplines, from the
sciences, through the humanities to law.  It also means that a CSL
style description can be used with any other application that
understands the CSL language.  And it means that the style description
is separated to the extent possible from the target document; you can
switch styles at any time, even after the writing process is complete.
Generality, a comprehensive pooling of community resources,
user-centric ease of use: all areas where, collectively, our current
menagerie of productivity tools could do better.

CSL first saw wide application in the Zotero project. [#]_
``Citeproc-js`` has been developed in the first instance for use in
Zotero, [#]_ but it runs as a separate module via a (relatively)
simple API, and with appropriate wrappers, it can be deployed pretty
much anywhere.  Potentially, any application that generates dynamic
content |mdash| text processors, word processors, weblog environments,
and dynamic websites |mdash| can use CSL and ``citeproc-js`` to
provide publisher-correct citation and bibliography facilities without
exceptional programming effort. [#]_

Development of the CSL language has been spearheaded by `Bruce D'Arcus`__.
The ``citeproc-js`` processor adheres to version 1.0 of the CSL
specification, [#]_ which has been engineered and documented during
the past year primarily by Bruce and `Rintze Zelle`__, with incidental
contributions by myself and others.  It will debut, together with the
new processor, in Zotero 2.1, which should begin to emerge, if all
goes well, during this calendar year.

__ http://community.muohio.edu/blogs/darcusb/

__ http://nl.linkedin.com/in/rintzezelle

Meanwhile, the processor itself is complete, documented, and more or
less ready to go. [#]_ Here is a short run-down of some of the highlights:

**Disambiguation**
    In author-date citation styles, works by the same author
    must be distinguished from one another in some way.  The
    current Zotero processor performs name and cite disambiguation
    as required by the Chicago Manual of Style.  There are in fact
    at least six other disambiguation methods in general use.
    CSL 1.0 and the new processor will support all of them.

**Sorting**
    The AGU journals, in particular, impose extremely demanding
    sorting rules in the bibliography. [#]_ CSL 1.0 and the new
    processor support multiple sort keys with arbitrary
    sort order for each key.  A wide variety of sorting schemes
    can be implemented, including the AGU sort.

**Parallel citation support**
    Many legal styles, including the Bluebook style common in American
    law journals, require that law cases appearing in multiple
    reporters be cited to each reporter, with the case name in front,
    and the court and year of decision at the end. [#]_  The new processor
    supports this behavior.

**On-the-fly document updates**
    The API of the new processor supports targeted context-sensitive
    updates of citations in a document that are affected by an
    insertion, deletion or edit, for efficient transactions with
    a word-processor or weblog plugin.

**Localization of dates**
    CSL version 0.8 currently supports the use of localized terms for
    style-supplied labels and the like.  CSL 1.0 will add
    sophisticated localization of dates; both the language of month
    names and the ordering and formatting of elements will adjust
    appropriately when the language of a citation style is changed.

**Sophisticated names handling**
    A great deal of work has gone into enhancing the handling of names
    in CSL 1.0.  European conventions on the handling of particles
    such as "von", "van", "di" and the like can be accounted for
    appropriately both in the sorting and in the rendering of
    individual names.

**In-field formatting**
    For scientific publishing, the new processor recognizes a limited
    subset of HTML as markup within titles, enabling superscript,
    subscript, small capitals, italics, boldface.  The processor
    also implements the flip-flopping of italic and boldface, and of
    quotation marks, to avoid ambiguity in rendered citations.  The
    HTML used in markup is transformed by the processor into the
    selected output format (HTML, RTF, LaTeX, or whatever) during rendering.

**Multi-lingual citation support**
    The new processor implements experimental support for multi-lingual
    citations, providing a flexible mechanism for the transliteration of 
    names and titles, for the supplementary translation of titles, and
    for the use of alternative sort strings needed for Asian languages.

As it leaves my laptop, ``citeproc-js`` is just a bare Javascript
module with some test suite wrappers to check that it actually
performs as advertised.  But with the widening availability and
increasing efficiency of Javascript runtime tools, I do hope that it
has some prospect of escaping from its cage and wreaking order on the
world of bibliography management.

=========================

.. [#] *See* Patashnik, "BibTeX yesterday, today, and tomorrow", TUGboat,
       v.24, n.1, p. 27 (2003) [`PDF`__] (accessed 2010.01.17).

__ http://www.tug.org/TUGboat/Articles/tb24-1/patashnik.pdf 

.. [#] The flavor of challenges to inter-operation in BibTeX is
       conveyed in a `post to the Zotero Forums (user noksagt, January 15, 2010)`__.  For
       an overview of the barriers in word processor
       environments, see Ginsburg, 
       "Unified Citation Management and Visualization Using Open Standards:
       The Open Citation System", J. of IT Standards & Standardization Research, 
       v.2, n.1, pp. 23-41 at 25-26 (2004) [`PDF`__] (accessed 2010.01.17).

__ http://forums.zotero.org/discussion/10603/bibtex-import-book-with-field-pages/#Comment_50785

__ http://www.infosci-journals.com/downloadPDF/pdf/ITJ2516_JQ62S0dPIQ.pdf

.. [#] CSL is also used by the `Mendeley`__ bibliography system.

__ http://www.mendeley.com/

.. [#] See the background summary provided in `Bennett, citeproc-js repository on
       BitBucket`__ (accessed 2010.01.17).

__ http://bitbucket.org/fbennett/citeproc-js/wiki/Home

.. [#] Note that CSL is larger than ``citeproc-js``, which is just one
       implementation of the standard.  In fact, development of ``citeproc-js``
       was inspired in part by the Haskell implementation of CSL 0.8, done
       by Andrea Rossato.  *See* `Rossato, "citeproc-hs - A Haskell
       Implementation of the Citation Style Language" (online document, 2008)`__ (accessed 2010.01.17).

__ http://code.haskell.org/citeproc-hs/

.. [#] As of this writing, the CSL version 1.0 schema has been tagged at ``rc2``.
       See `D'Arcus, CSL Schema repository on BitBucket`__ (accessed 2010.01.17).

__ http://bitbucket.org/bdarcus/csl-schema/src/

.. [#] *See* `Bennett, "Citation Style Language: Manual for the citeproc-js Processor"`__
       (accessed 2010.01.17)

__ http://gsl-nagoya-u.net/http/pub/citeproc-doc.html

.. [#] *See* "AGU Reference Style", p. 3 (online document, Apr. 9, 2009) [`PDF`__] 
   (accessed 2010.01.17).

__ http://www.agu.org/pubs/authors/manuscript_tools/journals/pdf/AGU_reference_style.pdf

.. [#] *E.g.*, *People v. Taylor*, 73 N.Y.2d 683, 690, 541 N.E.2d 
       386, 389, 543 N.Y.S.2d 357, 360 (1989) (this example from "The Bluebook:
       A Uniform System of Citation", P.3 [Columbia Law Review Ass'n et al. eds., 
       17th ed. 2000]).
