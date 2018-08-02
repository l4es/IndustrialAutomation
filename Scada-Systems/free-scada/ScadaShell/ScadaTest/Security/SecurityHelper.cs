using System;
using System.Linq;
using System.Collections.Generic;
using System.Security;
using System.Security.Principal;
using System.Threading;
// TODO: Use Web Security
using System.Web.Security;
// TODO: Use EntLib Security
using Microsoft.Practices.EnterpriseLibrary.Security;
using Microsoft.Practices.EnterpriseLibrary.Common.Configuration;
using FreeSCADA.Common;

namespace ScadaShell.Security
{
    class SecurityHelper
    {
        private SecurityHelper() 
        { }

        public static bool Authenticate(string username, string password)
        {
            bool authenticated = false;

            // TODO: Authenticate Credentials
            authenticated = Membership.ValidateUser(username, password);

            if (!authenticated)
                return false;

            IIdentity identity;
            identity = new GenericIdentity(username, Membership.Provider.Name);

            // TODO: Get Roles
            string[] roles = Roles.GetRolesForUser(identity.Name);
            IPrincipal principal = new GenericPrincipal(identity, roles);

            // Place user's principal on the thread
            //AppDomain.CurrentDomain.SetPrincipalPolicy(PrincipalPolicy.WindowsPrincipal);
            //if(Thread.CurrentPrincipal.Identity.Name=="")
               //AppDomain.CurrentDomain.SetThreadPrincipal(principal);
            //else
            Thread.CurrentPrincipal = principal;
            Env.Current.CurrentPrincipal = principal;
             
            return authenticated;
        }

        public static bool Authorized(string rule)
        {
            bool authorized = false;

            // TODO: Check rule-base authorization
            // No parameter passed to GetInstance method as 
            // we'll set the Default Authorization Instance in App.config.
            IAuthorizationProvider ruleProvider = EnterpriseLibraryContainer.Current.GetInstance<IAuthorizationProvider>();
            if (rule == null)
                rule = "View";
            //authorized = ruleProvider.Authorize(Thread.CurrentPrincipal, rule);
            authorized = ruleProvider.Authorize(Env.Current.CurrentPrincipal, rule);

            return authorized;
        }
        public static List<String> GetAllUsers()
        {
            MembershipUserCollection users=Membership.GetAllUsers();

            return users.Cast<MembershipUser>().Select(mu => mu.UserName).ToList();

        }
        public static bool ChangePassword(string userName,string oldPassword,string newPasswd)
        {
            MembershipUser u=Membership.GetUser(userName, false);
            return u.ChangePassword(oldPassword, newPasswd);
        }


    }
}
