//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
simple delaysender
{
    parameters:
        int Duration = default(10);
    gates:
        input in[];
        output out[];
        //inout neighbor[3];
}



network delsender
{
    @display("bgb=801,350");
    submodules:
        node1: delaysender {
            @display("p=105,58");
        }
        node2: delaysender {
            @display("p=288,58");
        }
        node3: delaysender {
            @display("p=117,204");
        }
        node4: delaysender {
            @display("p=299,204");
        }
    connections:
        node1.out++ --> node2.in++;
        node2.out++ --> node1.in++;
        node1.out++ --> node3.in++;
        node3.out++ --> node1.in++;
        node3.out++ --> node4.in++;
        node4.out++ --> node3.in++;
        node4.out++ --> node2.in++;
        node2.out++ --> node4.in++;
        node1.out++ --> node4.in++;
        node4.out++ --> node1.in++;
        node3.out++ --> node2.in++;
        node2.out++ --> node3.in++;
}