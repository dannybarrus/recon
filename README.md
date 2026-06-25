# recon

Why the name? A hiring manager I was working with had an email address 
that was a reference to his background as a Marine scout and forward 
observer. It felt like a fitting nod -- both to his service and to 
what I hoped to learn from someone with that kind of discipline and 
attention to detail. The name stuck because it fits the work: examine 
things carefully in the field as a means to improve.

The philosophy here is to approach problems the way a developer 
approaches unfamiliar territory -- with curiosity, deliberate 
examination, and a willingness to go deeper than the problem requires. 
You will find varying degrees of investigation, and notes where I 
intend to come back to things. This is as much about what piques my 
curiosity as anything else.

**On comments:** Most are omitted by design. The goal is to return to 
code cold and reconstruct the reasoning from the code itself. If you 
cannot, the code was not clear enough. This keeps the work honest.

Some of these implementations also serve as reference examples when 
onboarding engineers or conducting code reviews, and I draw from this 
repo when forming technical interview questions -- it helps me think 
clearly about what separates a working answer from a deep one.

## What's here

| Folder  | Covers |
|---------|--------|
| `cs/`   | C# -- async patterns, coding challenges, DI/IoC, OOP, SOLID, testing, REST API |
| `tf/`   | Terraform -- fundamentals through production, Azure and AWS including parallel AKS/EKS builds |
| `go/`   | Go -- Bloom filter CLI and Monte Carlo simulation engine |
| `c/`    | C -- CMake/Unity systems library: lock-free concurrency, embedded register and firmware patterns, custom allocators, zero-copy parsing, error-handling architectures |
| `cpp/`  | C++ -- CMake/GoogleTest reference library: algorithms, data structures, RAII and smart pointers, move semantics, templates |
| `java/` | Java -- Maven-based reference library: algorithms, data structures, collections, streams, OOP, exceptions, concurrency |

## Origin

The cpp and java folders came first, rooted in interview preparation. 
A coding challenge gave the repo its name and philosophy. The C# 
reference library, Terraform implementations, and Go projects are the 
most recent layer -- and both cpp and java have since grown well past 
their original interview-prep scope into fully tested reference 
libraries of their own, built on that same foundation.

The c folder is the exception to all of that: it isn't here for 
interview prep at all, just personal preference. C is my favorite 
language, full stop, and it earned a showcase on that basis alone.

The IoC container in cs/IoC/ is where the Recon name started. 
Worth a look.