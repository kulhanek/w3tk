#include <Template.hpp>
#include <XMLPrinter.hpp>
#include <ErrorSystem.hpp>
#include <TemplatePreprocessor.hpp>
#include <TemplateParams.hpp>

int main(void)
{
    CTemplate      temp;
    CXMLDocument   out;

    temp.OpenTemplate("test.tmp");

    CTemplatePreprocessor tempp;
    CTemplateParams params;

    params.Initialize();
    params.SetParam("TEST","mytest");
    params.Finalize();

    tempp.SetInputTemplate(&temp);
    tempp.SetOutputDocument(&out);

    tempp.PreprocessTemplate(&params);

    CXMLPrinter xml_printer;
    xml_printer.SetPrintedXMLNode(&out);

    xml_printer.Print(stdout);

    ErrorSystem.PrintErrors();
    return(0);
}

