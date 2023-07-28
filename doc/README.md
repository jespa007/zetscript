References
==========

- [Quick guide on syntaxes](http://asciidoctor.org/docs/asciidoc-syntax-quick-reference/)


Build
=====

## Requirements

### Install Ruby Version Manager (rvm)


From its source page [https://rvm.io/](rvm.io) do the following steps,

 1. Install GPG keys

```bash
gpg2 --recv-keys 409B6B1796C275462A1703113804BB82D39DC0E3 7D2BAF1CF37B13E2069D6956105BD0E739499BDB
```
 2. Install RVM

```bash
\curl -sSL https://get.rvm.io | bash -s stable

```

### Install Ruby 3.1

To install Ruby 3.1 do the follow,


```bash

rvm get head
rvm install 3.1
```
Check version ruby is 3.X

```bash
ruby --version
```

### Install asciidoctor


Install asciidoctor-pdf using the following steps,


```bash
gem install asciidoctor-pdf
```

Check asciidoctor version,

```bash
asciidoctor-pdf --version
```


## Build documentation


To build pdf documentation do,

```bash
asciidoctor-pdf zetscript-reference-manual.adoc
```



