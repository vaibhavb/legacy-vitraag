﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.4952
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

using System.Xml.Serialization;

// 
// This source code was auto-generated by xsd, Version=2.0.50727.42.
// 

namespace Microsoft.Health.Mobile
{

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.Xml.Serialization.XmlTypeAttribute(AnonymousType = true, Namespace = "urn:com.microsoft.wc.methods.CreateAuthenticatedSessionToken2", TypeName="info")]
    [System.Xml.Serialization.XmlRootAttribute(Namespace = "urn:com.microsoft.wc.methods.CreateAuthenticatedSessionToken2", IsNullable = false, ElementName="info")]
    public partial class CreateAuthenticatedSessionToken
    {

        private AuthTokenInfo authinfoField;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("auth-info", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public AuthTokenInfo authinfo
        {
            get
            {
                return this.authinfoField;
            }
            set
            {
                this.authinfoField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.methods.CreateAuthenticatedSessionToken2")]
    public partial class AuthTokenInfo
    {

        private AppInfo appidField;

        private Credential credentialField;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("app-id", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public AppInfo appid
        {
            get
            {
                return this.appidField;
            }
            set
            {
                this.appidField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public Credential credential
        {
            get
            {
                return this.credentialField;
            }
            set
            {
                this.credentialField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.types")]
    public partial class AppInfo
    {

        private bool ismultirecordappField;

        private bool ismultirecordappFieldSpecified;

        private string valueField;

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute("is-multi-record-app")]
        public bool ismultirecordapp
        {
            get
            {
                return this.ismultirecordappField;
            }
            set
            {
                this.ismultirecordappField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool ismultirecordappSpecified
        {
            get
            {
                return this.ismultirecordappFieldSpecified;
            }
            set
            {
                this.ismultirecordappFieldSpecified = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlTextAttribute()]
        public string Value
        {
            get
            {
                return this.valueField;
            }
            set
            {
                this.valueField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.types")]
    public partial class HMACAlgorithmData
    {

        private string algNameField;

        private string valueField;

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public string algName
        {
            get
            {
                return this.algNameField;
            }
            set
            {
                this.algNameField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlTextAttribute()]
        public string Value
        {
            get
            {
                return this.valueField;
            }
            set
            {
                this.valueField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.types")]
    public partial class HMACAlgorithm
    {

        private HMACAlgorithmData hmacalgField;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("hmac-alg", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public HMACAlgorithmData hmacalg
        {
            get
            {
                return this.hmacalgField;
            }
            set
            {
                this.hmacalgField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.auth")]
    public partial class UserPassAuthSessionCred
    {

        private string usernameField;

        private string passwordField;

        private HMACAlgorithm sharedsecretField;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public string username
        {
            get
            {
                return this.usernameField;
            }
            set
            {
                this.usernameField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public string password
        {
            get
            {
                return this.passwordField;
            }
            set
            {
                this.passwordField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("shared-secret", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public HMACAlgorithm sharedsecret
        {
            get
            {
                return this.sharedsecretField;
            }
            set
            {
                this.sharedsecretField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.methods.CreateAuthenticatedSessionToken2")]
    public partial class AppServerCredInfo
    {

        private string appidField;

        private hmac hmacField;

        private System.DateTime signingtimeField;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("app-id", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public string appid
        {
            get
            {
                return this.appidField;
            }
            set
            {
                this.appidField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public hmac hmac
        {
            get
            {
                return this.hmacField;
            }
            set
            {
                this.hmacField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("signing-time", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public System.DateTime signingtime
        {
            get
            {
                return this.signingtimeField;
            }
            set
            {
                this.signingtimeField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.methods.CreateAuthenticatedSessionToken2")]
    public enum hmac
    {

        /// <remarks/>
        HMACSHA256,

        /// <remarks/>
        HMACSHA1,
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.types")]
    public partial class Sig
    {

        private string digestMethodField;

        private string sigMethodField;

        private string thumbprintField;

        private string valueField;

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public string digestMethod
        {
            get
            {
                return this.digestMethodField;
            }
            set
            {
                this.digestMethodField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public string sigMethod
        {
            get
            {
                return this.sigMethodField;
            }
            set
            {
                this.sigMethodField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public string thumbprint
        {
            get
            {
                return this.thumbprintField;
            }
            set
            {
                this.thumbprintField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlTextAttribute()]
        public string Value
        {
            get
            {
                return this.valueField;
            }
            set
            {
                this.valueField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.methods.CreateAuthenticatedSessionToken2")]
    public partial class AppServerCred
    {

        private Sig sigField;

        private AppServerCredInfo contentField;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public Sig sig
        {
            get
            {
                return this.sigField;
            }
            set
            {
                this.sigField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute(Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public AppServerCredInfo content
        {
            get
            {
                return this.contentField;
            }
            set
            {
                this.contentField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.methods.CreateAuthenticatedSessionToken2")]
    public partial class Credential
    {

        private object itemField;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("appserver2", typeof(AppServerCred), Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        [System.Xml.Serialization.XmlElementAttribute("userpassauthsession", typeof(UserPassAuthSessionCred), Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public object Item
        {
            get
            {
                return this.itemField;
            }
            set
            {
                this.itemField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.auth")]
    [System.Xml.Serialization.XmlRootAttribute("auth", Namespace = "urn:com.microsoft.wc.auth", IsNullable = false)]
    public partial class Auth
    {

        private Rule[] rulesField;

        /// <remarks/>
        [System.Xml.Serialization.XmlArrayAttribute(Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        [System.Xml.Serialization.XmlArrayItemAttribute("rule", Form = System.Xml.Schema.XmlSchemaForm.Unqualified, IsNullable = false)]
        public Rule[] rules
        {
            get
            {
                return this.rulesField;
            }
            set
            {
                this.rulesField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.auth")]
    public partial class Rule
    {

        private CultureSpecificString1024[] reasonField;

        private uint displayflagsField;

        private bool displayflagsFieldSpecified;

        private Permission[] permissionField;

        private Set[] targetsetField;

        private Set[] exceptionsetField;

        private bool isincrField;

        private bool isincrFieldSpecified;

        private bool isoptionalField;

        private bool isoptionalFieldSpecified;

        private string nameField;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("reason", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public CultureSpecificString1024[] reason
        {
            get
            {
                return this.reasonField;
            }
            set
            {
                this.reasonField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("display-flags", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public uint displayflags
        {
            get
            {
                return this.displayflagsField;
            }
            set
            {
                this.displayflagsField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool displayflagsSpecified
        {
            get
            {
                return this.displayflagsFieldSpecified;
            }
            set
            {
                this.displayflagsFieldSpecified = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("permission", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public Permission[] permission
        {
            get
            {
                return this.permissionField;
            }
            set
            {
                this.permissionField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("target-set", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public Set[] targetset
        {
            get
            {
                return this.targetsetField;
            }
            set
            {
                this.targetsetField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("exception-set", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public Set[] exceptionset
        {
            get
            {
                return this.exceptionsetField;
            }
            set
            {
                this.exceptionsetField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute("is-incr")]
        public bool isincr
        {
            get
            {
                return this.isincrField;
            }
            set
            {
                this.isincrField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool isincrSpecified
        {
            get
            {
                return this.isincrFieldSpecified;
            }
            set
            {
                this.isincrFieldSpecified = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute("is-optional")]
        public bool isoptional
        {
            get
            {
                return this.isoptionalField;
            }
            set
            {
                this.isoptionalField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool isoptionalSpecified
        {
            get
            {
                return this.isoptionalFieldSpecified;
            }
            set
            {
                this.isoptionalFieldSpecified = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public string name
        {
            get
            {
                return this.nameField;
            }
            set
            {
                this.nameField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.types")]
    public partial class CultureSpecificString1024
    {

        private string langField;

        private string valueField;

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute(Form = System.Xml.Schema.XmlSchemaForm.Qualified, Namespace = "http://www.w3.org/XML/1998/namespace")]
        public string lang
        {
            get
            {
                return this.langField;
            }
            set
            {
                this.langField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlTextAttribute()]
        public string Value
        {
            get
            {
                return this.valueField;
            }
            set
            {
                this.valueField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.types")]
    public enum Permission
    {

        /// <remarks/>
        All,

        /// <remarks/>
        Read,

        /// <remarks/>
        Update,

        /// <remarks/>
        Create,

        /// <remarks/>
        Delete,
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.auth")]
    public partial class Set
    {

        private DateRange[] daterangeField;

        private string[] typeidField;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("date-range", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public DateRange[] daterange
        {
            get
            {
                return this.daterangeField;
            }
            set
            {
                this.daterangeField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("type-id", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public string[] typeid
        {
            get
            {
                return this.typeidField;
            }
            set
            {
                this.typeidField = value;
            }
        }
    }

    /// <remarks/>
    [System.CodeDom.Compiler.GeneratedCodeAttribute("xsd", "2.0.50727.42")]
    [System.SerializableAttribute()]
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.ComponentModel.DesignerCategoryAttribute("code")]
    [System.Xml.Serialization.XmlTypeAttribute(Namespace = "urn:com.microsoft.wc.types")]
    public partial class DateRange
    {

        private System.DateTime dateminField;

        private bool dateminFieldSpecified;

        private System.DateTime datemaxField;

        private bool datemaxFieldSpecified;

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("date-min", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public System.DateTime datemin
        {
            get
            {
                return this.dateminField;
            }
            set
            {
                this.dateminField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool dateminSpecified
        {
            get
            {
                return this.dateminFieldSpecified;
            }
            set
            {
                this.dateminFieldSpecified = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlElementAttribute("date-max", Form = System.Xml.Schema.XmlSchemaForm.Unqualified)]
        public System.DateTime datemax
        {
            get
            {
                return this.datemaxField;
            }
            set
            {
                this.datemaxField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlIgnoreAttribute()]
        public bool datemaxSpecified
        {
            get
            {
                return this.datemaxFieldSpecified;
            }
            set
            {
                this.datemaxFieldSpecified = value;
            }
        }
    }
}