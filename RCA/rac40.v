// 40-bit rca
// use five 8-bit RCAs
module rca_40b(S, A, B, Cout, Cin);
	// input
   input [39:0] A, B;
   input Cin;
	
	// output
   output [39:0] S;
   output Cout;
   
	// carry between 8-bit RCAs
   wire [3:0] Carry;
   
	// Instantion : five 8-bit RCAs
	// Carry, which occurred in the previous RCA, is used as Cin in the next RCA
	// The Cout of the last RCA is the final output round
   rca_8b rca_8b01(.sum(S[7:0]), .a(A[7:0]), .b(B[7:0]), .Cin(Cin), .Cout(Carry[0])); 
   rca_8b rca_8b02(.sum(S[15:8]), .a(A[15:8]), .b(B[15:8]), .Cin(Carry[0]), .Cout(Carry[1]));
   rca_8b rca_8b03(.sum(S[23:16]), .a(A[23:16]), .b(B[23:16]), .Cin(Carry[1]), .Cout(Carry[2]));
   rca_8b rca_8b04(.sum(S[31:24]), .a(A[31:24]), .b(B[31:24]), .Cin(Carry[2]), .Cout(Carry[3]));
   rca_8b rca_8b05(.sum(S[39:32]), .a(A[39:32]), .b(B[39:32]), .Cin(Carry[3]), .Cout(Cout));
endmodule


// Full-adder
module FullAdder(sum, a, b, Cin, Cout);
	// input
   input a, b, Cin;
	
	// output
   output sum, Cout;
   
	// Calculate sum & Cout
   assign sum = a ^ b ^ Cin;
   assign Cout = (a & b) | (a & Cin) | (b & Cin);
endmodule


// 4-bit rca
// use four Full-adders
module rca_4b(sum, a, b, Cin, Cout);
	// input
   input [3:0] a, b;
   input Cin;
   
	// output
   output [3:0] sum; 
   output Cout;
   
	// carry between Full-adders
   wire [2:0] Carry;
   
	// Instantion : four Full Adders
	// Carry, which occurred in the previous RCA, is used as Cin in the next RCA
	// The Cout of the last RCA is the final output round
   FullAdder FA01(.sum(sum[0]), .a(a[0]), .b(b[0]), .Cin(Cin), .Cout(Carry[0]));
   FullAdder FA02(.sum(sum[1]), .a(a[1]), .b(b[1]), .Cin(Carry[0]), .Cout(Carry[1]));
   FullAdder FA03(.sum(sum[2]), .a(a[2]), .b(b[2]), .Cin(Carry[1]), .Cout(Carry[2]));
   FullAdder FA04(.sum(sum[3]), .a(a[3]), .b(b[3]), .Cin(Carry[2]), .Cout(Cout));
endmodule


// 8-bit rca
// use two 4-bit RCAs
module rca_8b(sum, a, b, Cin, Cout);
	// input
   input [7:0] a, b;
   input Cin;
   
	// output
   output [7:0] sum;
   output Cout;
   
	// carry between 4-bit RCAs
   wire Carry;
   
	// Instantiation : two 4-bit RCAs
	// Carry, which occurred in the previous RCA, is used as Cin in the next RCA
	// The Cout of the last RCA is the final output round
   rca_4b rca_4b01(.sum(sum[3:0]), .a(a[3:0]), .b(b[3:0]), .Cin(Cin), .Cout(Carry));
   rca_4b rca_4b02(.sum(sum[7:4]), .a(a[7:4]), .b(b[7:4]), .Cin(Carry), .Cout(Cout));
endmodule
