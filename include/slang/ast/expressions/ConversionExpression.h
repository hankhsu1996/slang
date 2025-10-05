//------------------------------------------------------------------------------
//! @file ConversionExpression.h
//! @brief Definitions for conversion expressions
//
// SPDX-FileCopyrightText: Michael Popoloski
// SPDX-License-Identifier: MIT
//------------------------------------------------------------------------------
#pragma once

#include "slang/ast/Expression.h"

namespace slang::ast {

/// Represents a type conversion expression (implicit or explicit).
class SLANG_EXPORT ConversionExpression final : public Expression {
public:
    /// The kind of conversion.
    ConversionKind conversionKind;

    bool isConstCast = false;

    ConversionExpression(const Type& type, ConversionKind conversionKind, Expression& operand,
                         SourceRange sourceRange) :
        Expression(ExpressionKind::Conversion, type, sourceRange), conversionKind(conversionKind),
        operand_(&operand) {}

    /// @returns true if this is an implicit conversion
    bool isImplicit() const { return conversionKind < ConversionKind::StreamingConcat; }

    /// @returns the operand of the conversion
    const Expression& operand() const { return *operand_; }

    /// @returns the operand of the conversion
    Expression& operand() { return *operand_; }

    ConstantValue evalImpl(EvalContext& context) const;
    std::optional<bitwidth_t> getEffectiveWidthImpl() const;
    EffectiveSign getEffectiveSignImpl(bool isForConversion) const;

    ConstantValue applyTo(EvalContext& context, ConstantValue&& value) const;

    void serializeTo(ASTSerializer& serializer) const;

    static Expression& fromSyntax(Compilation& compilation,
                                  const syntax::CastExpressionSyntax& syntax,
                                  const ASTContext& context, const Type* assignmentTarget);
    static Expression& fromSyntax(Compilation& compilation,
                                  const syntax::SignedCastExpressionSyntax& syntax,
                                  const ASTContext& context);

    static Expression& makeImplicit(const ASTContext& context, const Type& targetType,
                                    ConversionKind conversionKind, Expression& expr,
                                    const Expression* parentExpr, SourceRange opRange);

    static ConstantValue convert(EvalContext& context, const Type& from, const Type& to,
                                 SourceRange sourceRange, ConstantValue&& value,
                                 ConversionKind conversionKind, const Expression* expr = nullptr,
                                 SourceRange implicitOpRange = {});

    static void checkImplicitConversions(const ASTContext& context, const Type& from,
                                         const Type& to, const Expression& expr,
                                         const Expression* parentExpr, SourceRange opRange,
                                         ConversionKind conversionKind);

    static bool isKind(ExpressionKind kind) { return kind == ExpressionKind::Conversion; }

    /// @returns the width expression for size casts (e.g., NUM_ENTRIES in NUM_ENTRIES'(value))
    /// Returns nullptr for type casts or implicit conversions
    const Expression* getCastWidthExpr() const { return castWidthExpr_; }

    /// Sets the width expression for LSP symbol tracking
    void setCastWidthExpr(const Expression* expr) { castWidthExpr_ = expr; }

    template<typename TVisitor>
    decltype(auto) visitExprs(TVisitor&& visitor) const {
        return operand().visit(visitor);
    }

private:
    Expression* operand_;
    SourceRange implicitOpRange;

    /// Bound expression for size cast width (for LSP symbol tracking)
    /// For casts like NUM_ENTRIES'(value), this stores the NUM_ENTRIES expression
    const Expression* castWidthExpr_ = nullptr;
};

} // namespace slang::ast
