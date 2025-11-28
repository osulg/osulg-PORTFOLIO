`timescale 1ns/100ps

module tb_rca_40b;
	// input registers
   reg [39:0] A, B;
   reg Cin;
	
	// output wires
   wire [39:0] S;
   wire Cout;

   rca_40b uut (.S(S), .A(A), .B(B),.Cin(Cin), .Cout(Cout));

   initial begin
    // Test Case 1
     A = 40'b0000_0000_0000_0000_0000_0000_0000_0000_0000_0001;
     B = 40'b0000_0000_0000_0000_0000_0000_0000_0000_0000_0001;
     Cin = 1'b0;
     #10;
	  // Displayed in binary and decimal numbers
     $display("Test Case 1: A=%b, B=%b, Cin=%b => S=%b, Cout=%b", A, B, Cin, S, Cout);
	  $display("Test Case 1: A=%d, B=%d, Cin=%b => S=%d, Cout=%b", A, B, Cin, S, Cout);
 

     // Test Case 2
     A = 40'b0000_0000_0000_0000_0000_0000_0000_0000_0000_0010;
     B = 40'b0000_0000_0000_0000_0000_0000_0000_0000_0000_0011;
     Cin = 1'b1;
     #10;
	  // Displayed in binary and decimal numbers
     $display("Test Case 2: A=%b, B=%b, Cin=%b => S=%b, Cout=%b", A, B, Cin, S, Cout);
  	  $display("Test Case 2: A=%d, B=%d, Cin=%b => S=%d, Cout=%b", A, B, Cin, S, Cout);


     // Test Case 3
     A = 40'b0000_0000_0000_0000_0000_0000_0000_0000_0000_1111;
     B = 40'b0000_0000_0000_0000_0000_0000_0000_0000_0000_0001;
     Cin = 1'b0;
     #10;
	  // Displayed in binary and decimal numbers
     $display("Test Case 3: A=%b, B=%b, Cin=%b => S=%b, Cout=%b", A, B, Cin, S, Cout);
	  $display("Test Case 3: A=%d, B=%d, Cin=%b => S=%d, Cout=%b", A, B, Cin, S, Cout);

     // Test Case 4
     A = 40'b0000_0000_0000_0000_0000_0000_0000_0000_0000_0000;
     B = 40'b1111_1111_1111_1111_1111_1111_1111_1111_1111_1111;
     Cin = 1'b0;
     #10;
     $display("Test Case 4: A=%b, B=%b, Cin=%b => S=%b, Cout=%b", A, B, Cin, S, Cout);
	  $display("Test Case 4: A=%d, B=%d, Cin=%b => S=%d, Cout=%b", A, B, Cin, S, Cout);

	 
     // Test Case 5
     A = 40'b0000_0000_0000_0000_0000_0000_0000_0000_0000_0001;
     B = 40'b1111_1111_1111_1111_1111_1111_1111_1111_1111_1111;
     Cin = 1'b0;
     #10;
	  // Displayed in binary and decimal numbers
     $display("Test Case 5: A=%b, B=%b, Cin=%b => S=%b, Cout=%b", A, B, Cin, S, Cout);
	  $display("Test Case 5: A=%d, B=%d, Cin=%b => S=%d, Cout=%b", A, B, Cin, S, Cout);


     // Test Case 6
     A = 40'b1111_1111_1111_1111_1111_1111_1111_1111_1111_1111;
     B = 40'b1111_1111_1111_1111_1111_1111_1111_1111_1111_1111;
     Cin = 1'b1;
     #10;
	  // Displayed in binary and decimal numbers
     $display("Test Case 6: A=%b, B=%b, Cin=%b => S=%b, Cout=%b", A, B, Cin, S, Cout);
	  $display("Test Case 6: A=%d, B=%d, Cin=%b => S=%d, Cout=%b", A, B, Cin, S, Cout);
    
	end
endmodule
