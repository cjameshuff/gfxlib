
#ifndef CPPUTILS_H
#define CPPUTILS_H

#include <functional>
#include <utility>

// Do an action when a variable goes out of scope.
// Useful for doing cleanup in code that has multiple opportunities
// to produce errors and abort.
//
// Example:
// void ReadFile(string & fname) {
//     ifstream fin(fname);
//     if(!fin)
//         throw exception("couldn't open file");
//     
//     AtDescope([&]{fin.close();});
//     
//     CheckHeader(fin);// may throw an exception
//     
//     ReadData(fin);// may also throw an exception
//     
// }

template<typename fn_t>
class AtDescope {
    fn_t fn;
  public:
    AtDescope(fn_t _f): fn(_f) {}
    ~AtDescope() {fn();}
};


#endif // CPPUTILS_H
