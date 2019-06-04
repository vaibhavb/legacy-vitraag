<%@ Page Language="C#" AutoEventWireup="true"  MasterPageFile="~/HelloWorld.master" CodeFile="Default.aspx.cs" Inherits="HelloWorldPage" %>

<asp:Content ID="Content1" ContentPlaceHolderID="PageBody" runat="server">

<div id="header">
    Welcome <asp:Label ID="Lbl_UserName" runat="server"></asp:Label>,
</div>

<div>
  Weight Values<br />
    <asp:GridView ID="WeightView" runat="server">
    </asp:GridView>
</div>

<div>
Record today's weight (in lbs):
    <asp:TextBox ID=Txt_Weight runat="server"></asp:TextBox>
    <asp:Button ID="Btn_SubmitWeight" runat="server" Text="Submit" 
        onclick="Btn_SubmitWeight_Click"/>
    <asp:Button ID="Btn_SubmitAndSignWeight" runat="server" Text="Submit and Sign" 
        onclick="Btn_SubmitAndSignWeight_Click"/>
</div>

</asp:Content>
