#include "mfem.hpp"
#include <fstream>
#include <iostream>
#include <chrono>

using namespace std;
using namespace mfem;

const auto f_right = [](const mfem::Vector &v) {
	return v(0) * v(1);
};

template<typename precision>
class Timer {
public:
	Timer(): begin(std::chrono::steady_clock::now()) {}
	
	~Timer() {
		std::cout << std::chrono::duration_cast<precision>(std::chrono::steady_clock::now() - begin).count() << std::endl;
	}
	
	std::chrono::steady_clock::time_point begin;
};


int main(int argc, char *argv[]) {
	for (auto refs = 0; refs < 9; refs++){
		
    	const char *mesh_file = "./star.mesh";
    	int order = 1;
    	bool static_cond = false;

    	Mesh *mesh = new Mesh(mesh_file, 1, 1);
    	int dim = mesh->Dimension();

    	for (int l = 0; l < refs; l++) {
            mesh->UniformRefinement();
        }
        
        std::cout << mesh->GetNE() << ' ';
    
    	auto begin = std::chrono::steady_clock::now();

		{
			Timer<std::chrono::microseconds> t;
    		FiniteElementCollection *fec = new H1_FECollection(order, dim);
    		FiniteElementSpace *fespace = new FiniteElementSpace(mesh, fec);

    		Array<int> ess_tdof_list;
    		Array<int> ess_bdr;
    		ess_bdr.SetSize(mesh->bdr_attributes.Max());
    		ess_bdr = 1;
    		fespace->GetEssentialTrueDofs(ess_bdr, ess_tdof_list);

    		LinearForm b(fespace);
    		ConstantCoefficient one(1.);
    		FunctionCoefficient b_function(f_right);
    		b.AddDomainIntegrator(new DomainLFIntegrator(b_function));
    		b.Assemble();

    		GridFunction x(fespace);
    		x = 0.0;

    		BilinearForm a(fespace);
    		a.AddDomainIntegrator(new DiffusionIntegrator(one));
    		a.Assemble();

    		OperatorPtr A;
    		Vector B, X;
    		a.FormLinearSystem(ess_tdof_list, x, b, A, X, B);

	

    		UMFPackSolver umf_solver;
    		umf_solver.Control[UMFPACK_ORDERING] = UMFPACK_ORDERING_METIS;
    		umf_solver.SetOperator(*A);
    		umf_solver.Mult(B, X);

    		a.RecoverFEMSolution(X, b, x);
    
    	}

	}
    return 0;
}
