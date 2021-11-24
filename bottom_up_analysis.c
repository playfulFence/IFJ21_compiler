#include "bottom_up_analysis.h"





ptElement ptFromTokenToPTElement(tokenType_t type)
{

}

int ptGetIndex(ptElement element)
{

}

token_t bottomUpAnalysis(token_t token)
{
    StackTokens tokensStack;
    initStackTokens(&tokensStack);
    PTStack ptElementsStack;
    initStackPTElements(&ptElementsStack);
    DynamicString ruleSequenceString;
    DynamicStringInit(&ruleSequenceString);
    
}