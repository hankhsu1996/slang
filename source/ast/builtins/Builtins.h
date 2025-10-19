//------------------------------------------------------------------------------
// Builtins.h
// Container type for canonical instance of various built-in types and methods
//
// SPDX-FileCopyrightText: Michael Popoloski
// SPDX-License-Identifier: MIT
//------------------------------------------------------------------------------

#include <memory>
#include <string_view>
#include <tuple>

#include "slang/ast/SystemSubroutine.h"
#include "slang/ast/types/AllTypes.h"
#include "slang/util/FlatMap.h"

namespace slang::ast::builtins {

class Builtins {
public:
    ScalarType bitType;
    ScalarType logicType;
    ScalarType regType;
    ScalarType signedBitType;
    ScalarType signedLogicType;
    ScalarType signedRegType;
    PredefinedIntegerType intType;
    PredefinedIntegerType uintType;
    PredefinedIntegerType byteType;
    PredefinedIntegerType integerType;
    PredefinedIntegerType shortIntType;
    PredefinedIntegerType longIntType;
    PredefinedIntegerType ulongIntType;
    PredefinedIntegerType timeType;
    FloatingType realType;
    FloatingType shortRealType;
    FloatingType realTimeType;
    StringType stringType;
    VoidType voidType;
    CHandleType chandleType;
    NullType nullType;
    EventType eventType;
    UnboundedType unboundedType;
    TypeRefType typeRefType;
    UntypedType untypedType;
    SequenceType sequenceType;
    PropertyType propertyType;
    ErrorType errorType;

    std::vector<std::shared_ptr<SystemSubroutine>> systemSubroutines;
    flat_hash_map<std::string_view, std::shared_ptr<SystemSubroutine>> subroutineNameMap;
    flat_hash_map<std::tuple<std::string_view, SymbolKind>, std::shared_ptr<SystemSubroutine>>
        methodMap;

    explicit Builtins(Compilation& compilation) :
        bitType(ScalarType::Bit, compilation), logicType(ScalarType::Logic, compilation),
        regType(ScalarType::Reg, compilation), signedBitType(ScalarType::Bit, true, compilation),
        signedLogicType(ScalarType::Logic, true, compilation),
        signedRegType(ScalarType::Reg, true, compilation),
        intType(PredefinedIntegerType::Int, compilation),
        uintType(PredefinedIntegerType::Int, false, compilation),
        byteType(PredefinedIntegerType::Byte, compilation),
        integerType(PredefinedIntegerType::Integer, compilation),
        shortIntType(PredefinedIntegerType::ShortInt, compilation),
        longIntType(PredefinedIntegerType::LongInt, compilation),
        ulongIntType(PredefinedIntegerType::LongInt, false, compilation),
        timeType(PredefinedIntegerType::Time, compilation),
        realType(FloatingType::Real, compilation),
        shortRealType(FloatingType::ShortReal, compilation),
        realTimeType(FloatingType::RealTime, compilation), stringType(compilation),
        voidType(compilation), chandleType(compilation), nullType(compilation),
        eventType(compilation), unboundedType(compilation), typeRefType(compilation),
        untypedType(compilation), sequenceType(compilation), propertyType(compilation),
        errorType(compilation) {
        systemSubroutines.resize(parsing::KnownSystemName_traits::values.size());

        registerArrayMethods();
        registerConversionFuncs();
        registerCoverageFuncs();
        registerEnumMethods();
        registerMathFuncs();
        registerMiscSystemFuncs();
        registerNonConstFuncs();
        registerQueryFuncs();
        registerStringMethods();
        registerSystemTasks();
    }

private:
    using KnownSystemName = parsing::KnownSystemName;

    void registerArrayMethods();
    void registerConversionFuncs();
    void registerCoverageFuncs();
    void registerEnumMethods();
    void registerMathFuncs();
    void registerMiscSystemFuncs();
    void registerNonConstFuncs();
    void registerQueryFuncs();
    void registerStringMethods();
    void registerSystemTasks();

    void addSystemSubroutine(std::shared_ptr<SystemSubroutine> subroutine) {
        SLANG_ASSERT(subroutine->knownNameId != KnownSystemName::Unknown);
        SLANG_ASSERT(!systemSubroutines[(size_t)subroutine->knownNameId]);
        subroutineNameMap.emplace(subroutine->name, subroutine);
        systemSubroutines[(size_t)subroutine->knownNameId] = std::move(subroutine);
    }

    void addSystemMethod(SymbolKind typeKind, std::shared_ptr<SystemSubroutine> method) {
        methodMap.emplace(std::make_tuple(std::string_view(method->name), typeKind),
                          std::move(method));
    }
};

} // namespace slang::ast::builtins
