using System;
using System.Collections.Generic;

using System.Web;

using Microsoft.Health;
using Microsoft.Health.Web;
using Microsoft.Health.ItemTypes;
using System.Security.Cryptography.X509Certificates;

public partial class HelloWorldPage : HealthServicePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        Lbl_UserName.Text = this.PersonInfo.SelectedRecord.DisplayName;

        HealthRecordSearcher searcher = PersonInfo.SelectedRecord.CreateSearcher();

        
        HealthRecordFilter filter = new HealthRecordFilter(Weight.TypeId);
        filter.MaxItemsReturned = 10;
        filter.UpdatedDateMin = DateTime.Now.Subtract(new TimeSpan(365, 0, 0, 0));                    
        searcher.Filters.Add(filter);

        HealthRecordItemCollection items = searcher.GetMatchingItems()[0];

        Dictionary<string, string> weights = new Dictionary<string, string>();

        foreach (Weight item in items)
        {
            weights[item.When.ToString()] = item.Value.ToString();
        }

        WeightView.DataSource = weights;
        WeightView.DataBind();
    }
    
    
    protected void Btn_SubmitWeight_Click(object sender, EventArgs e)
    {
        double weight = double.Parse(Txt_Weight.Text);
        Weight w = new Weight( 
                new HealthServiceDateTime(DateTime.Now),
                new WeightValue(
                    weight * 1.6, new DisplayValue(weight, "lbs", "lbs")));
        
        PersonInfo.SelectedRecord.NewItem(w);
    }

    protected void Btn_SubmitAndSignWeight_Click(object sender, EventArgs e)
    {
        double weight = double.Parse(Txt_Weight.Text);
        Weight w = new Weight(
                new HealthServiceDateTime(DateTime.Now),
                new WeightValue(
                    weight * 1.6, new DisplayValue(weight, "lbs", "lbs")));

        X509Certificate2 cert = new X509Certificate2();
        cert.Import("..\\cert\valid_cert.pfx");
        
        w.Sign(cert);
        PersonInfo.SelectedRecord.NewItem(w);

    }
}
