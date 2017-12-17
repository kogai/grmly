type browser;

type page;

type buffer;

type element;

type request = {. "resourceType": string};

type response = {. "text": unit => Js.Promise.t(string), "url": string, "status": int};

[@bs.module "puppeteer"]
external launch : Js.undefined({. "headless": Js.boolean}) => Js.Promise.t(browser) =
  "launch";

[@bs.send] external newPage : browser => Js.Promise.t(page) = "newPage";

[@bs.send] external goto : (page, ~url: string) => Js.Promise.t(response) = "goto";

[@bs.send] external screenshot : (page, {. "path": string}) => Js.Promise.t(buffer) = "screenshot";

[@bs.send] external type_ : (page, string, string, option({. delay: int})) => Js.Promise.t(unit) =
  "type";

[@bs.send]
external click :
  (page, string, option({. button: string, clickCount: int, delay: int})) => Js.Promise.t(element) =
  "click";

[@bs.send] external waitFor : (page, int) => Js.Promise.t(unit) = "waitFor";

[@bs.send] external waitForFunction : (page, string) => Js.Promise.t(unit) = "waitForFunction";

[@bs.send]
external waitForSelector :
  (page, string, Js.undefined({. "hidden": Js.boolean})) => Js.Promise.t(unit) =
  "waitForSelector";

[@bs.send] external on : (page, string, response => 'b) => Js.Promise.t(unit) = "on";

[@bs.send] external close : browser => Js.Promise.t(unit) = "close";

let url = "https://app.grammarly.com/";

let signIn = "https://www.grammarly.com/signin";

let wsEndpoint = "wss://capi.grammarly.com/freews";

let _ = {
  open Js.Promise;
  let (email, password) =
    try (Sys.getenv("GRMLY_EMAIL"), Sys.getenv("GRMLY_PASSWORD")) {
    | Not_found =>
      Printf.printf("Environment variables GRMLY_EMAIL and GRMLY_PASSWORD are mandatory");
      raise(Not_found)
    };
  let sentence = "I'd encountered same problem, and a dugg down into a code, then I thinks that I found what the reason it caused.";
  let _ =
    launch(Js.Undefined.return({"headless": Js.false_}))
    |> then_(
         (browser) =>
           browser
           |> newPage
           |> then_(
                (page) => {
                  Js.log("OK");
                  page
                  |> goto(~url=signIn)
                  |> then_((_) => type_(page, "input[type=email]", email, None))
                  |> then_((_) => type_(page, "input[type=password]", password, None))
                  |> then_((_) => click(page, "button[type=submit]", None))
                  |> then_(
                       (_) =>
                         waitForSelector(page, "div[data-name=new-doc-item]", Js.Undefined.empty)
                     )
                  |> then_((_res) => screenshot(page, {"path": "screenshot/logged-in.png"}))
                  |> then_((_) => click(page, "div[data-name=new-doc-add-btn]", None))
                  |> then_((_) => waitForSelector(page, "textarea#textarea", Js.Undefined.empty))
                  |> then_(
                       (_) => {
                         let _ =
                           on(
                             page,
                             "response",
                             (res) => {
                               /* wss://dox.grammarly.com/documents/243288472/ws */
                               let url = res##url;
                               let isMatch =
                                 Js.Re.fromString("^https://dox.grammarly.com/documents");
                               if (Js.Re.test(url, isMatch)) {
                                 Js.log(res##url)
                               }
                             }
                           );
                         type_(page, "textarea#textarea", sentence, None)
                       }
                     )
                  |> then_((_res) => screenshot(page, {"path": "screenshot/inputed.png"}))
                  |> then_(
                       (_) =>
                         waitForSelector(
                           page,
                           "span[data-name=checking]",
                           Js.Undefined.return({"hidden": Js.true_})
                         )
                     )
                  |> then_((_res) => screenshot(page, {"path": "screenshot/checked.png"}))
                  |> then_((_) => close(browser))
                }
              )
       );
  ()
};
