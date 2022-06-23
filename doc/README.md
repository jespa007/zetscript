Template for Writing an eBook
=============================

- Has book template(asciidoc) for creating html, pdf, epub/mobi.
- Uses [asciidoctor](http://asciidoctor.org) to make the book.
- See `master.adoc`


Installation
------------

```bash
git clone git://github.com/vysakh0/asciidoctor-book-template.git
cd asciidoctor-book-template
bundle
rake
```

Usage
-----

- Use `rake ascii` to build html, pdf, epub files for your master.adoc
- Outputs will be generated under the _output_ directory.

Roll on
-------
- [Quick guide on syntaxes](http://asciidoctor.org/docs/asciidoc-syntax-quick-reference/)
- Start editing, adding the  `about`, `preface`, `chapters` and others.
