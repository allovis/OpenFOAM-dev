/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2020 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::fv::CodedSource

Description
   Constructs on-the-fly fvOption source from user-supplied code

Usage
    Example usage in controlDict:
    \verbatim
    energySource
    {
        type            scalarCodedSource;

        name    sourceTime;

        scalarCodedSourceCoeffs
        {
            selectionMode   all;

            fields          (h);

            codeInclude
            #{
            #};

            codeCorrect
            #{
                Pout<< "**codeCorrect**" << endl;
            #};

            codeAddSup
            #{
                const Time& time = mesh().time();
                const scalarField& V = mesh_.V();
                scalarField& heSource = eqn.source();
                heSource -= 0.1*sqr(time.value())*V;
            #};

            codeAddRhoSup
            #{
                Pout<< "**codeAddRhoSup**" << endl;
            #};

            codeAddAlphaRhoSup
            #{
                Pout<< "**codeAddAlphaRhoSup**" << endl;
            #};

            codeSetValue
            #{
                Pout<< "**codeSetValue**" << endl;
            #};
        }

        sourceTimeCoeffs
        {
            $scalarCodedSourceCoeffs;
        }
    }
    \endverbatim


SourceFiles
    codedSource.C

\*---------------------------------------------------------------------------*/

#ifndef CodedSource_H
#define CodedSource_H

#include "cellSetOption.H"
#include "codedBase.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                         Class codedSource Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class CodedSource
:
    public cellSetOption,
    public codedBase
{
protected:

    // Protected static data

        //- The keywords associated with source code
        static const wordList codeKeys_;


    // Protected data

        //- The name
        word name_;

        //- Underlying functionObject
        mutable autoPtr<option> redirectFvOptionPtr_;


    // Protected Member Functions

        //- Adapt the context for the current object
        virtual void prepare(dynamicCode&, const dynamicCodeContext&) const;

        //- Name of the dynamically generated CodedType
        virtual const word& codeName() const
        {
            return name_;
        }

        //- Return a description (type + name) for the output
        virtual string description() const;

        //- Clear any redirected objects
        virtual void clearRedirect() const;

        //- Get the dictionary to initialize the codeContext
        virtual const dictionary& codeDict() const;

        //- Get the keywords associated with source code
        virtual const wordList& codeKeys() const;


public:

    //- Runtime type information
    TypeName("coded");


    // Constructors

        //- Construct from components
        CodedSource
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );


    // Member Functions

        //- Dynamically compiled fvOption
        option& redirectFvOption() const;


        // Evaluation

            //- Correct field
            virtual void correct
            (
                GeometricField<Type, fvPatchField, volMesh>&
            ) const;

            //- Explicit and implicit matrix contributions
            virtual void addSup
            (
                fvMatrix<Type>& eqn,
                const label fieldi
            ) const;

            //- Explicit and implicit matrix contributions
            //  to compressible equation
            virtual void addSup
            (
                const volScalarField& rho,
                fvMatrix<Type>& eqn,
                const label fieldi
            ) const;

            //- Explicit and implicit matrix contributions
            //  to phase equation
            virtual void addSup
            (
                const volScalarField& alpha,
                const volScalarField& rho,
                fvMatrix<Type>& eqn,
                const label fieldi
            ) const;

            //- Set value
            virtual void constrain
            (
                fvMatrix<Type>& eqn,
                const label fieldi
            ) const;


        // IO

            //- Read source dictionary
            virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "CodedSource.C"
    #include "CodedSourceIO.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
