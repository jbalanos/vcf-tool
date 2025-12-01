#include <iostream>
#include <vcf_tool/domain/greeting.h>

int main() {
    std::cout << "VCF Tool v0.1.0" << std::endl;
    std::cout << vcf_tool::domain::hello_from_domain() << std::endl;
    return 0;
}
